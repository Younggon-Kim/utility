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

#ifndef TIMER_HPP__
#define TIMER_HPP__

#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <atomic>

#include "logger.hpp"
#include "event.hpp"

namespace util {

class Timer final {
public:
    Timer() : event_(std::make_shared<util::Event>()),
              is_running_(false) {}

    using TIMER_HANDLER = std::function<void ()>;

    /**
     * Set timer which will invoke handler every given `interval` time
     *
     * @tparam Rep an arithmetic type representing the number of ticks
     * @tparam Period a std::ratio representing the tick period (i.e. the number of seconds per tick)
     * @param handler callback function to be invoked every given `interval` time
     * @param interval the time interval at which handler is desired to be called
     */
    template<typename Rep, typename Period>
    void setInterval(TIMER_HANDLER handler, std::chrono::duration<Rep, Period> interval) {
        std::lock_guard<std::mutex> lock(mutex_);

        if(is_running_) {
            LOG_WARN("Timer is already ticking..");
            return;
        }

        is_running_ = true;

        std::thread([=](){
            while(this->is_running_) {
                if(this->event_->wait_for(interval)) {
                    this->event_->reset();
                    return;
                }
                if(this->is_running_ && handler) {
                    handler();
                }
                this->event_->reset();
            }
        }).detach();
    }

    /**
     * Set timer which will invoke handler after the specific `timeout` duration
     *
     * @tparam Rep an arithmetic type representing the number of ticks
     * @tparam Period a std::ratio representing the tick period (i.e. the number of seconds per tick)
     * @param handler callback function to be invoked after the given `timeout` time
     * @param timeout elapsed time when the handler is called
     */
    template<typename Rep, typename Period>
    void setTimeout(TIMER_HANDLER handler, std::chrono::duration<Rep, Period> timeout) {
        std::lock_guard<std::mutex> lock(mutex_);

        if(is_running_) {
            LOG_WARN("Timer is already ticking..");
            return;
        }

        is_running_ = true;

        std::thread([=]() {
            if(this->event_->wait_for(timeout)) {
                this->event_->reset();
                return;
            }
            if(handler) {
                handler();
            }
            this->event_->reset();
            this->is_running_ = false;
        }).detach();
    }

    /**
     * Stop timer
     */
    void stop() {
        is_running_ = false;
        event_->cancel();
    }

    bool isRunning() {
        return is_running_;
    }

private:
    std::mutex mutex_;
    std::shared_ptr<util::Event> event_;
    std::atomic<bool> is_running_;
};

} //namespace util

#endif //TIMER_HPP__
