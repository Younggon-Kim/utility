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

#ifndef SINGLETON_HPP__
#define SINGLETON_HPP__

#include <mutex>

namespace util {

template<typename T>
class Singleton {
public:
    static T& getInstance() noexcept {
        std::lock_guard<std::mutex> lock(s_mutex);

        T *instance = s_instance;
        if(instance == nullptr) {
            instance = new T();
            s_instance = instance;
        }
        return *s_instance;
    }

    static bool hasInstance() noexcept {
        std::lock_guard<std::mutex> lock(s_mutex);

        return s_instance != nullptr;
    }

protected:
    ~Singleton() { };
    Singleton() { };

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator = (const Singleton&) = delete;
    static std::mutex s_mutex;
    static T* s_instance;
};

template<typename T> T* Singleton<T>::s_instance = nullptr;
template<typename T> std::mutex Singleton<T>::s_mutex;

} //namespace util

#endif //SINGLETON_HPP__