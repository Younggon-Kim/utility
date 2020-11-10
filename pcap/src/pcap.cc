/*
 * Copyright (C) 2020  Younggon Kim<dev.ygkim@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <thread>
#include <future>
#include <cstring>

#include "pcap.hpp"
#include "logger.hpp"

namespace util {

Pcap::Pcap(uint8_t size) : handle_(nullptr),
                           dump_t_(nullptr),
                           blocked_(false),
                           qsize(size == 0 ? 100 : size) {}

Pcap::~Pcap() {
    terminate();
}

bool Pcap::init(const std::string ifName, const std::string filter) {
    if(ifName.length() == 0) {
        LOG_WARN("Invalid ifName length");
        return false;
    }

    if(handle_) {
        LOG_WARN("Pcap is already initialized");
        return false;
    }

    char errBuf[PCAP_ERRBUF_SIZE];
    int snapshot_len = 1500;
    int promiscuous = 0; //promiscuuos mode
    int timeout = 1000; //1 sec
    struct bpf_program fp; //The compiled filter
    bpf_u_int32 mask; // netmask
    bpf_u_int32 net; // IP

    // Open the session in promiscuous mode
    handle_ = pcap_open_live(ifName.c_str(), snapshot_len, promiscuous, timeout, errBuf);

    if(!handle_) {
        LOG_ERROR("Fail to create pcap, %s", errBuf);
        return false;
    }

    // Set nonblocking mode
    if(pcap_setnonblock(handle_, 1, errBuf) != 0) {
        LOG_ERROR("Fail to setnonblock, %s", errBuf);
        return false;
    }

    if(filter.length() > 0) {
        LOG_INFO("Pcap ifName : %s, filter : %s", ifName.c_str(), filter.c_str());
        // Find the properties for the device
        if (pcap_lookupnet(ifName.c_str(), &net, &mask, errBuf) == -1) {
            LOG_ERROR("Fail to get netmask for device %s: %s", ifName.c_str(), errBuf);
            net = 0;
            mask = 0;
            return false;
        }

        // Compile and apply the filter
        if (pcap_compile(handle_, &fp, filter.c_str(), 0, net) == -1) {
            LOG_ERROR("Fail to parse filter %s: %s", filter.c_str(), pcap_geterr(handle_));
            return false;
        }

        if (pcap_setfilter(handle_, &fp) == -1) {
            LOG_ERROR("Fail to install filter %s: %s", filter.c_str(), pcap_geterr(handle_));
            return false;
        }
    }

    LOG_DEBUG("Initialization is succssful(%s)", ifName.c_str());

    return true;
}

void Pcap::terminate() {
    stop();

    if(handle_) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

bool Pcap::dump(const std::string fileName) {
    if(fileName.length() == 0) {
        LOG_ERROR("Invalid file name length");
        return false;
    }

    if(!handle_) {
        LOG_ERROR("Pcap is not initialized");
        return false;
    }

    if(dump_t_) {
        LOG_ERROR("dump is already running");
        return false;
    }

    const std::string extension = ".pcap";

    dump_t_ = pcap_dump_open(handle_, (fileName + extension).c_str());

    if(!dump_t_) {
        LOG_ERROR("Fail to open pcap_dump_open, %s", pcap_geterr(handle_));
        return false;
    }

    dump_thread_ = std::make_shared<std::thread>([&] {
        while(pcap_dispatch(this->handle_, 0, &pcap_dump, (u_char *)this->dump_t_) >= 0) {
            //do nothing
        }

        pcap_dump_close(this->dump_t_);
        this->dump_t_ = nullptr;
    });

    return true;
}

bool Pcap::dump(PCAP_HANDLER handler) {
    if(!handle_) {
        LOG_ERROR("Pcap is not initialized");
        return false;
    }

    if(dump_thread_.get()) {
        LOG_ERROR("dump is already running");
        return false;
    }

    dump_thread_ = std::make_shared<std::thread>([&] {
        pcap_loop(this->handle_, 0, &Pcap::pcapHandler_, reinterpret_cast<u_char*>(this));
    });

    blocked_ = true;

    notify_thread_ = std::make_shared<std::thread>([&, handler] {
        std::unique_lock<std::mutex> lock(this->mutex_);

        while(this->blocked_) {
            this->condition_.wait(lock);

            //If it has been triggered to stop, we don't need to invoke handler
            if(!this->blocked_) {
                return;
            }

            while(!this->queue_.empty()) {
                auto packet = this->queue_.front();

                struct pcap_pkthdr *pcap_hdr = std::get<0>(packet);
                uint8_t *data = std::get<1>(packet);

                if(handler) {
                    handler(pcap_hdr, data);
                }

                this->queue_.pop();

                if(pcap_hdr) {
                    free(pcap_hdr);
                }
                if(data) {
                    free(data);
                }
            }
        }
    });

    return true;
}

void Pcap::stop() {
    //break pcap_loop
    if(handle_) {
        pcap_breakloop(handle_);
    }

    if(dump_thread_.get() && dump_thread_->joinable()) {
        dump_thread_->join();
        dump_thread_.reset();
    }

    //interrupt notify thread
    blocked_ = false;
    condition_.notify_all();

    if(notify_thread_.get() && notify_thread_->joinable()) {
        notify_thread_->join();
        notify_thread_.reset();
    }

    //clear queue
    while(!queue_.empty()) {
        auto packet = queue_.front();
        queue_.pop();

        auto hdr = std::get<0>(packet);
        if(hdr) {
            free(hdr);
        }
        auto data = std::get<1>(packet);
        if(data) {
            free(data);
        }
    }
}

void Pcap::pcapHandler_(u_char *user, const struct pcap_pkthdr *header, const u_char *bytes) {
    if(user == nullptr) {
        return;
    }

    Pcap *self = reinterpret_cast<Pcap*>(user);

    if(self == nullptr) {
        return;
    }

    //limit queue size
    if(self->queue_.size() >= self->qsize) {
        return;
    }

    //copy pcap_header, bytes
    struct pcap_pkthdr *pcap_hdr = (struct pcap_pkthdr *)malloc(sizeof(struct pcap_pkthdr));
    if(pcap_hdr == nullptr) {
        LOG_WARN("Cannot allocate memory for copying packet");
        return;
    }
    std::memcpy(pcap_hdr, header, sizeof(struct pcap_pkthdr));

    uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t) * header->caplen);
    if(data == nullptr) {
        LOG_WARN("Cannot allocate memory for copying packet");
        if(pcap_hdr) {
            free(pcap_hdr);
        }
        return;
    }
    std::memcpy(data, bytes, header->caplen);

    //enqueue
    self->queue_.push(std::make_pair(pcap_hdr, data));
    self->condition_.notify_one();
}

} //namespace util
