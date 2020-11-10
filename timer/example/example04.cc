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
 * This example describes how to set time interval with given time and reuse it.
 * The callback delivered to setInterval() will be called every interval time.
 */
class Monitor {
public:
    Monitor() : is_running_(false),
                retry_(0),
                cnt_(0) {}

    void run(std::chrono::milliseconds interval, int retry) {
        if(is_running_) {
            LOG_WARN("Monitor is already running");
            return;
        }

        LOG_INFO("Monitor is running");

        is_running_ = true;
        interval_ = interval;
        retry_ = retry;

        start();

        while(is_running_) {
            //do nothing
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void start() {
        LOG_INFO("Monitor is starting, current retry(%d)", retry_);
        timer_.setInterval(std::bind(&Monitor::onInterval, this), interval_);
    }

    void stop() {
        LOG_INFO("Monitor is stopping..");

        cnt_ = 0;
        timer_.stop();

        if(--retry_ > 0) {
            start();
        } else {
            is_running_ = false;
        }
    }

private:
    Timer timer_;
    std::chrono::milliseconds interval_;
    bool is_running_;
    int retry_;
    int cnt_;

    void onInterval() {
        LOG_INFO("time interval has been triggered, cnt_(%d), tid(%x)", cnt_, std::this_thread::get_id());

        if(++cnt_ > 5) {
            stop();
        }
    }
};

} //namespace util

int main(int argc, char **argv) {
    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    util::Monitor monitor;

    monitor.run(std::chrono::milliseconds(300), 4);
    monitor.stop();

    return 0;
}
