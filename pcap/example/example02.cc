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

#include <string>
#include <thread>
#include <chrono>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "pcap.hpp"
#include "logger.hpp"

namespace util {

/**
 * This example describes how to capture packets and save them into file
 */
class Monitor {
public:
    bool init(std::string ifName, std::string filter, std::chrono::seconds timeout = std::chrono::seconds(10)) {
        if(ifName.length() == 0) {
            return false;
        }

        timeout_ = timeout;

        return pcap_.init(ifName, filter);
    }

    void capturePackets(std::string fileName) {
        if(!fileName.length()) {
            LOG_ERROR("Invalid filename");
            return;
        }
        pcap_.dump(fileName);

        std::thread t([&, this] {
            std::this_thread::sleep_for(this->timeout_);
        });
        t.join();

        pcap_.stop();
        pcap_.terminate();
    }

    void stop() {
        pcap_.stop();
        pcap_.terminate();
    }

private:
    std::chrono::seconds timeout_;
    util::Pcap pcap_;
};

} //namespace util

void usage() {
    fprintf(stderr, "Capture packets\n");
    fprintf(stderr, "Usage: ./example02 <options>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h help               print usage\n");
    fprintf(stderr, "  -i <Interface name>   Capture packets at <interface name>\n");
    fprintf(stderr, "  -f <filter statement> pcap filtering statement\n");
    fprintf(stderr, "                        e.g. TCP PORT 443\n");
    fprintf(stderr, "  -w <file name>        file name to save packet dump\n");
    exit(1);
}

int main(int argc, char **argv) {
    int opt;
    char *if_name_ = nullptr;
    char *filter_ = nullptr;
    char *savefile_ = nullptr;

    while ((opt = getopt(argc, argv, "hi:f:w:")) != -1) {
        switch(opt) {
            case 'h':
                usage();
                break;
            case 'i':
                if_name_ = optarg;
                break;
            case 'f':
                filter_ = optarg;
                break;
            case 'w':
                savefile_ = optarg;
                break;
        }
    }

    if(!if_name_) {
        fprintf(stderr, "[ERROR] Invalid interface name\n");
        usage();
        return -1;
    }

    if(!savefile_) {
        fprintf(stderr, "[ERROR] Invalid file name\n");
        usage();
        return -1;
    }

    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    util::Monitor monitor;
    std::string ifName = if_name_;
    std::string filter = ((filter_) ? std::string(filter_) : "");
    std::string fileName = savefile_;

    if(!monitor.init(ifName, filter)) {
        fprintf(stderr, "Fail to initialize monitor\n");
        return -1;
    }

    //capture packets 10 seconds
    monitor.capturePackets(fileName);

    return 0;
}
