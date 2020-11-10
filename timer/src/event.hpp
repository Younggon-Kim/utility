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

#ifndef EVENT_HPP__
#define EVENT_HPP__

#include <mutex>
#include <condition_variable>

namespace util {

class Event {
public:
    Event() : signaled_(false) {}

    void cancel() noexcept {
        std::unique_lock<std::mutex> lock(mutex_);

        signaled_ = true;
        cv_.notify_all();
    }

    template<typename T>
    bool wait_for(T t) noexcept {
        std::unique_lock<std::mutex> lock(mutex_);

        return cv_.wait_for(lock, t, [&](){
            return this->signaled_ != false;
        });
    }

    void reset() noexcept {
        signaled_ = false;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool signaled_;
};

} //namespace util

#endif //EVENT_HPP__
