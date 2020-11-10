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
 * This example describes how to set timeout with given time and reuse it.
 * The callback delivered to setTimeout() will be called after time is expired.
 */
class Monitor {
public:
    Monitor() : is_running_(false),
                cnt_(0) {}

    void run(std::chrono::milliseconds timeout, int retry) {
        if(is_running_) {
            LOG_WARN("Monitor is already running");
            return;
        }

        LOG_INFO("Monitor is running");

        is_running_ = true;
        cnt_ = retry;
        timeout_ = timeout;

        start();

        while(is_running_) {
            //do nothing
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void start() {
        LOG_INFO("Monitor is called Start, currnet cnt(%d)", cnt_);

        timer_.setTimeout(std::bind(&Monitor::onExpired, this), timeout_);
    }

    void stop() {
        LOG_INFO("Monitor is called Stop, current cnt(%d)", cnt_);

        //test calling stop() twice
        timer_.stop();
        timer_.stop();

        if(--cnt_ > 0) {
            start();
        } else {
            is_running_ = false;
        }
    }

private:
    Timer timer_;
    std::chrono::milliseconds timeout_;
    bool is_running_;
    int cnt_;

    void onExpired() {
        LOG_INFO("timeout has been expired, currnt cnt(%d)", cnt_);
        stop();
    }
};

} //namespace util

int main(int argc, char **argv) {
    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    util::Monitor monitor;

    monitor.run(std::chrono::milliseconds(1000 * 3), 3);

    return 0;
}
