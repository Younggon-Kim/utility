/*
 * Copyright (C) 2020  Younggon Kim<yg.david.kim@lge.com>
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
#include <chrono>

#include "timer.hpp"
#include "logger.hpp"

namespace util {

/**
 * This example describes how to set time interval with given time.
 * The callback delivered to setInterval() will be called every interval time.
 */
class Monitor {
public:
    Monitor() : is_running_(false) {}

    void run(std::chrono::milliseconds interval) {
        if(is_running_) {
            LOG_WARN("Monitor is already running");
            return;
        }

        LOG_INFO("Monitor is running");

        is_running_ = true;

        timer_.setInterval(std::bind(&Monitor::onInterval, this), interval);

        std::thread t([] {
            std::this_thread::sleep_for(std::chrono::seconds(5));
        });
        t.join();
    }

    void stop() {
        is_running_ = false;
        timer_.stop();
    }

private:
    Timer timer_;
    bool is_running_;

    void onInterval() {
        LOG_INFO("time interval has been triggered");
    }
};

} //namespace util

int main(int argc, char **argv) {
    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    util::Monitor monitor;

    monitor.run(std::chrono::milliseconds(1000));
    monitor.stop();

    return 0;
}
