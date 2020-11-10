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

#include <gtest/gtest.h>
#include <future>
#include <chrono>
#include <thread>
#include <ctime>
#include <atomic>

#include "timer.hpp"
#include "logger.hpp"

namespace util {

class TimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        //do nothing
    }

    void TearDown() override {
        //do nothing
    }
};

TEST_F(TimerTest, setInterval_within_duration) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    auto past = std::chrono::system_clock::now();

    Timer timer;
    timer.setInterval([&]{
        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(1500)) {
            p.set_value(true);
        } else {
            p.set_value(false);
        }
    }, std::chrono::seconds(1));

    f.wait();
    ASSERT_TRUE(f.get()) << "Interval should be triggered everty specific duration";

    timer.stop();
}

TEST_F(TimerTest, setInterval_nullptr_handler) {
    Timer timer;
    timer.setInterval(nullptr, std::chrono::seconds(1));

    std::this_thread::sleep_for(std::chrono::seconds(3));

    timer.stop();

    ASSERT_TRUE(true) << "Timer shoud be working without TIMER_HANDLER";
}

TEST_F(TimerTest, setInterval_reuse) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    int cnt = 0;
    std::atomic<bool> satisfied;
    satisfied.store(false);

    LOG_INFO("first timer interval is running.");
    Timer timer;
    timer.setInterval([&] {
        std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        LOG_INFO("first timer cnt : %d, Time : %s", ++cnt, std::ctime(&current));
        LOG_INFO("first timer, thread id : %ld", std::this_thread::get_id());

        if(cnt > 3 && !satisfied.load()) {
            satisfied.store(true);
            p.set_value(true);
        }
    }, std::chrono::seconds(1));

    f.wait();
    ASSERT_TRUE(f.get()) << "Timer shoud be working";

    timer.stop();

    //re-use
    LOG_INFO("second timer interval is running.");

    std::promise<bool> p2;
    std::future<bool> f2 = p2.get_future();
    cnt = 0;
    satisfied.store(false);

    timer.setInterval([&] {
        std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        LOG_INFO("second timer cnt : %d, Time : %s", ++cnt, std::ctime(&current));
        LOG_INFO("second timer, thread id : %ld", std::this_thread::get_id());

        if(cnt > 3 && !satisfied.load()) {
            satisfied.store(true);
            p2.set_value(true);
        }
    }, std::chrono::seconds(1));

    f2.wait();
    ASSERT_TRUE(f2.get()) << "Timer shoud be reusable";

    timer.stop();
}

TEST_F(TimerTest, setInterval_reuse_short_duration) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    int cnt = 0;
    std::atomic<bool> satisfied;
    satisfied.store(false);

    LOG_INFO("first timer interval is running.");
    Timer timer;
    timer.setInterval([&] {
        std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        LOG_INFO("first timer cnt : %d, Time : %s", ++cnt, std::ctime(&current));
        LOG_INFO("first timer, thread id : %ld", std::this_thread::get_id());

        if(cnt > 10 && !satisfied.load()) {
            satisfied.store(true);
            p.set_value(true);
        }
    }, std::chrono::milliseconds(10));

    f.wait();
    ASSERT_TRUE(f.get()) << "Timer shoud be working";

    timer.stop();

    //re-use
    LOG_INFO("second timer interval is running.");

    std::promise<bool> p2;
    std::future<bool> f2 = p2.get_future();
    cnt = 0;
    satisfied.store(false);

    timer.setInterval([&] {
        std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        LOG_INFO("second timer cnt : %d, Time : %s", ++cnt, std::ctime(&current));
        LOG_INFO("second timer, thread id : %ld", std::this_thread::get_id());

        if(cnt > 10 && !satisfied.load()) {
            satisfied.store(true);
            p2.set_value(true);
        }
    }, std::chrono::milliseconds(10));

    f2.wait();
    ASSERT_TRUE(f2.get()) << "Timer shoud be reusable";

    timer.stop();
}

TEST_F(TimerTest, setTimeout_normal) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    auto past = std::chrono::system_clock::now();

    Timer timer;
    timer.setTimeout([&] {
        LOG_INFO("timeout has been expired");

        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(3500)) {
            p.set_value(true);
        } else {
            p.set_value(false);
        }
    }, std::chrono::seconds(3));

    f.wait();
    ASSERT_TRUE(f.get()) << "setTimeout should be triggered after the specific duration";
}

TEST_F(TimerTest, setTimeout_nullptr_callback) {
    Timer timer;
    timer.setTimeout(nullptr, std::chrono::seconds(1));

    std::this_thread::sleep_for(std::chrono::seconds(3));

    timer.stop();

    ASSERT_TRUE(true) << "setTimeout shoud be working without TIMER_HANDLER";
}

TEST_F(TimerTest, setTimeout_reuse) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    auto past = std::chrono::system_clock::now();

    Timer timer;
    timer.setTimeout([&] {
        LOG_INFO("first timer timeout has been expired");

        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(3500)) {
            p.set_value(true);
        } else {
            p.set_value(false);
        }
    }, std::chrono::seconds(3));

    f.wait();
    ASSERT_TRUE(f.get()) << "setTimeout should be triggered after the specific duration";

    //reuse timer
    std::promise<bool> p2;
    std::future<bool> f2 = p2.get_future();

    past = std::chrono::system_clock::now();

    timer.setTimeout([&] {
        LOG_INFO("second timer timeout has been expired");

        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(3500)) {
            p2.set_value(true);
        } else {
            p2.set_value(false);
        }
    }, std::chrono::seconds(3));

    f2.wait();
    ASSERT_TRUE(f2.get()) << "setTimeout should be triggered after the specific duration";
}

TEST_F(TimerTest, setTimeout_reuse_short_duration) {
    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    auto past = std::chrono::system_clock::now();

    Timer timer;
    timer.setTimeout([&] {
        LOG_INFO("first timer timeout has been expired");

        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(20)) {
            p.set_value(true);
        } else {
            p.set_value(false);
        }
    }, std::chrono::milliseconds(10));

    f.wait();
    ASSERT_TRUE(f.get()) << "setTimeout should be triggered after the specific duration";

    //reuse timer
    std::promise<bool> p2;
    std::future<bool> f2 = p2.get_future();

    past = std::chrono::system_clock::now();

    timer.setTimeout([&] {
        LOG_INFO("second timer timeout has been expired");

        auto current = std::chrono::system_clock::now();
        auto diff = current - past;

        if(diff < std::chrono::milliseconds(20)) {
            p2.set_value(true);
        } else {
            p2.set_value(false);
        }
    }, std::chrono::milliseconds(10));

    f2.wait();
    ASSERT_TRUE(f2.get()) << "setTimeout should be triggered after the specific duration";
}

} //namespace util

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    return RUN_ALL_TESTS();
}
