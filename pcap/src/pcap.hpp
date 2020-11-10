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

#ifndef PCAP_HPP__
#define PCAP_HPP__

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <pcap.h>
#include <deque>
#include <utility>
#include <condition_variable>

#include "safe_queue.hpp"

namespace util {

class Pcap final {
public:
    using PCAP_HANDLER = std::function<void (const struct pcap_pkthdr *header, const uint8_t *packet)>;

    explicit Pcap(uint8_t size = 100);
    ~Pcap();

    /**
     * @brief Initialize Pcap object with interface name and filter expression.
     * @param ifName interface name to capture packets
     * @param filter filter expression specified as a text string. e.g. TCP port 443
     * @return return true if it is successful to initialize, otherwise false
     */
    bool init(const std::string ifName, const std::string filter = "");

    /**
     * @brief Terminate Pcap resources
     */
    void terminate();

    /**
     * @brief Dump live packets to a file with given name
     * @param fileName file name to save
     * @return return true if it is successful to start capturing packets, otherwise false
     */
    bool dump(const std::string fileName);

    /**
     * @brief Dump live packets to given callback function
     * @param handler Callback function to handle packet
     * @return return true if it is successful to start capturing packets, otherwise false
     */
    bool dump(PCAP_HANDLER handler);

    /**
     * @brief Stop capturing packets.
     *        Call terminate() after this, if you do not want to capture packets anymore.
     */
    void stop();

private:
    pcap_t *handle_;
    pcap_dumper_t *dump_t_;
    std::shared_ptr<std::thread> dump_thread_;
    std::shared_ptr<std::thread> notify_thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool blocked_;
    util::SafeQueue<std::pair<struct pcap_pkthdr*, uint8_t *>> queue_;
    uint8_t qsize;

    static void pcapHandler_(u_char *user, const struct pcap_pkthdr *header, const u_char *bytes);
};

} //namespace util

#endif //PCAP_HPP__
