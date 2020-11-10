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

#ifndef SAFE_QUEUE_HPP__
#define SAFE_QUEUE_HPP__

#include <queue>
#include <mutex>
#include <condition_variable>

namespace util {

template<typename T>
class SafeQueue final {
public:
    typedef typename std::queue<T>::value_type value_type;
    typedef typename std::queue<T>::reference reference;
    typedef typename std::queue<T>::const_reference const_reference;
    typedef typename std::queue<T>::size_type size_type;
    typedef typename std::queue<T>::container_type container_type;

    SafeQueue() {}
    ~SafeQueue() = default;

    //copy constructor
    explicit SafeQueue(const SafeQueue<T> &obj) : queue_(obj.queue_) {}

    //move constructor
    explicit SafeQueue(SafeQueue<T> &&obj) : queue_(std::move(obj.queue_)) {}

    T front() {
        std::unique_lock<std::mutex> lock(mutex_);
        auto item = queue_.front();
        lock.unlock();

        return item;
    }

    void pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.pop();
        lock.unlock();
        condition_.notify_one();
    }

    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        lock.unlock();
        condition_.notify_one();
    }

    void push(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(std::move(item));
        lock.unlock();
        condition_.notify_one();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    typename std::queue<T>::size_type size() const {
        return queue_.size();
    }

    inline bool operator==(const SafeQueue<T> &obj) {
        return this->queue_ == obj.queue_;
    }

    inline bool operator!=(const SafeQueue<T> &obj) {
        return !(this->queue_ == obj.queue_);
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

} //namespace util

#endif //SAFE_QUEUE_HPP__
