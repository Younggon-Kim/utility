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

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>

#include "pcap.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include "date_util.hpp"
#include "pcap_util.hpp"

namespace util {

class PcapTest : public ::testing::Test {
public:
    static std::string if_name;
    static std::chrono::seconds test_timeout;
    static std::chrono::seconds test_interval;

protected:
    void SetUp() override {
        //do nothing
    }

    void TearDown() override {
        //do nothing
    }
};

std::string PcapTest::if_name;
std::chrono::seconds PcapTest::test_timeout {5};
std::chrono::seconds PcapTest::test_interval {5};

TEST_F(PcapTest, init_invalid_ifname) {
    Pcap pcap;

    ASSERT_FALSE(pcap.init("")) << "Pcap::init should return false with invalid interface name";
}

TEST_F(PcapTest, init_valid_ifname) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";

    pcap.terminate();
}

TEST_F(PcapTest, init_valid_ifname_invalid_filter) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_FALSE(pcap.init(PcapTest::if_name, "invalid filter")) << "Pcap::init should return false with invalid filter";

    pcap.terminate();
}

TEST_F(PcapTest, init_valid_ifname_valid_filter) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_FALSE(pcap.init(PcapTest::if_name, "invalid filter")) << "Pcap::init should return false with invalid filter";

    pcap.terminate();
}

TEST_F(PcapTest, init_duplicated) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";

    ASSERT_FALSE(pcap.init(PcapTest::if_name)) << "Pcap::init should return false when it is intialized duplicated";
}

TEST_F(PcapTest, terminate_without_init) {
    Pcap pcap;

    try {
        pcap.terminate();
    } catch (...) {
        GTEST_FAIL() << "terminate should never be failed even if it was not initialized.";
    }
    GTEST_SUCCEED();
}

TEST_F(PcapTest, init_terminate_duplicated) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init return true with valid interface name";

    ASSERT_FALSE(pcap.init(PcapTest::if_name)) << "Pcap::init should return false when it is intialized duplicated";

    try {
        pcap.terminate();
        pcap.terminate();
    } catch (...) {
        GTEST_FAIL() << "terminate should never be failed even if it was terminated twice";
    }
    GTEST_SUCCEED();
}

TEST_F(PcapTest, dump2file_without_init) {
    Pcap pcap;

    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    ASSERT_FALSE(pcap.dump(filename)) << "Pcap::dump should return false without initialization";
}

TEST_F(PcapTest, dump2callback_without_init) {
    Pcap pcap;

    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    ASSERT_FALSE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        GTEST_FAIL() << "Pcap::dump should never invoke callback when it is triggered without initialization.";
    })) << "Pcap::dump should return false without initialization";
}

TEST_F(PcapTest, dump2file_normal) {
    Timer timer;
    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";

    //timeout
    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    //check dump file exists
    std::ifstream file(filename + ".pcap");
    ASSERT_TRUE(file.is_open()) << "dump file should exist";

    //check dump file size
    auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    auto end = file.tellg();
    auto size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file.close();
}

TEST_F(PcapTest, dump2file_invalid_name) {
    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_FALSE(pcap.dump("")) << "Pcap::dump should return false with invalid file name";
}

TEST_F(PcapTest, dump2file_duplicated) {
    Timer timer;
    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    Pcap pcap;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";
    ASSERT_FALSE(pcap.dump(filename)) << "Pcap::dump should return false when it is invoked twice";

    //timeout
    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    //check dump file exists
    std::ifstream file(filename + ".pcap");
    ASSERT_TRUE(file.is_open()) << "dump file should exist";

    //check dump file size
    auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    auto end = file.tellg();
    auto size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file.close();
}

TEST_F(PcapTest, dump2file_reuse) {
    Timer timer;
    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    Pcap pcap;

    LOG_INFO("first test...");
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";

    //timeout
    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    //check dump file exists
    std::ifstream file(filename + ".pcap");
    ASSERT_TRUE(file.is_open()) << "dump file should exist";

    //check dump file size
    auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    auto end = file.tellg();
    auto size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file.close();

    LOG_INFO("second test...");
    filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";

    //timeout
    running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    //check dump file exists
    std::ifstream file2(filename + ".pcap");
    ASSERT_TRUE(file2.is_open()) << "dump file should exist";

    //check dump file size
    begin = file2.tellg();
    file2.seekg(0, std::ios::end);
    end = file2.tellg();
    size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file2.close();
}

TEST_F(PcapTest, dump2file_reuse_without_terminate) {
    Timer timer;
    std::string filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);

    Pcap pcap;

    LOG_INFO("first test...");
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";

    //timeout
    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    //Note : we don't termiante Pcap.

    //check dump file exists
    std::ifstream file(filename + ".pcap");
    ASSERT_TRUE(file.is_open()) << "dump file should exist";

    //check dump file size
    auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    auto end = file.tellg();
    auto size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file.close();

    LOG_INFO("second test...");
    filename = util::DateUtil::getCurrentTimeStr(util::TIME_FORMAT::YYMMDD_HHMMSS);
    //Note : we don't need to initialize again because we didn't terminate pcap.
    ASSERT_TRUE(pcap.dump(filename)) << "Pcap::dump should return true with valid file name";

    //timeout
    running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    //check dump file exists
    std::ifstream file2(filename + ".pcap");
    ASSERT_TRUE(file2.is_open()) << "dump file should exist";

    //check dump file size
    begin = file2.tellg();
    file2.seekg(0, std::ios::end);
    end = file2.tellg();
    size = end - begin;

    LOG_INFO("file(%s) size(%u)", filename.c_str(), size);
    ASSERT_GT(size, 0);

    file2.close();
}

TEST_F(PcapTest, dump2callback_normal) {
    Timer timer;
    Pcap pcap;

    int cnt = 0;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("captured packet count : %d", cnt);
        PcapUtil::printPacket(header, packet);
    }));

    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";
}

TEST_F(PcapTest, dump2callback_duplicated) {
    Timer timer;
    Pcap pcap;

    int cnt = 0;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("captured packet count : %d", cnt);
        PcapUtil::printPacket(header, packet);
    }));
    ASSERT_FALSE(pcap.dump([] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        GTEST_FAIL() << "This callback should be never invoked";
    }));

    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";
}

TEST_F(PcapTest, dump2callback_reuse) {
    Timer timer;
    Pcap pcap;

    int cnt = 0;

    LOG_INFO("First test...");
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("First test captured packet count : %d, thread id : %u", cnt, std::this_thread::get_id());
        PcapUtil::printPacket(header, packet);
    }));

    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";

    cnt = 0;

    LOG_INFO("Second test...");
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("Second test captured packet count : %d, thread id : %u", cnt, std::this_thread::get_id());
        PcapUtil::printPacket(header, packet);
    }));

    running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";
}

TEST_F(PcapTest, dump2callback_reuse_without_terminate) {
    Timer timer;
    Pcap pcap;

    int cnt = 0;

    LOG_INFO("First test...");
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("First test captured packet count : %d, thread id : %u", cnt, std::this_thread::get_id());
        PcapUtil::printPacket(header, packet);
    }));

    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    //Note : we don't termiante Pcap.

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";

    cnt = 0;

    LOG_INFO("Second test...");
    //Note : we don't need to initialize again because we didn't terminate pcap.
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("Second test captured packet count : %d, thread id : %u", cnt, std::this_thread::get_id());
        PcapUtil::printPacket(header, packet);
    }));

    running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";
}

TEST_F(PcapTest, dump2callback_stop_twice) {
    Timer timer;
    Pcap pcap;

    int cnt = 0;

    //Note : make sure interface name on your system
    ASSERT_TRUE(pcap.init(PcapTest::if_name)) << "Pcap::init should return true with valid interface name";
    ASSERT_TRUE(pcap.dump([&] (const struct pcap_pkthdr *header, const uint8_t *packet) {
        ++cnt;
        // LOG_INFO("captured packet count : %d", cnt);
        PcapUtil::printPacket(header, packet);
    }));

    bool running = true;
    timer.setTimeout([&] {
        pcap.stop();
        running = false;
    }, test_timeout);

    while(running) {
        //waiting for timeout
    }

    //stop duplicated
    pcap.stop();

    pcap.terminate();

    ASSERT_GT(cnt, 0) << "Captured packet count should be greater than zero";
}

} //namespace util

void usage() {
    std::cerr << "=======================================================" << std::endl;
    std::cerr << "Usage : pcap_test <options>" << std::endl;
    std::cerr << "  --ifname <interface name> interface name to capture packets in the test" << std::endl;
    std::cerr << "  --timeout <seconds> unsigned duration to configure timeout on Timer" << std::endl;
    std::cerr << "  --interval <seconds> unsigned duration to configure interval duration on Timer" << std::endl;
    std::cerr << "=======================================================" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    int i = 1;

    while(i < argc) {
        if(argv[i] == std::string("--ifname")) {
            util::PcapTest::if_name = std::string(argv[i + 1]);
        } else if(argv[i] == std::string("--timeout")) {
            int input = std::stoi(argv[i + 1]);
            if(input <= 0) {
                std::cerr << "Invalid timeout duration" << std::endl;
                usage();
                exit(1);
            }

            util::PcapTest::test_timeout = std::chrono::seconds(input);
            LOG_INFO("Timeout setting : %u", util::PcapTest::test_timeout);
        } else if(argv[i] == std::string("--interval")) {
            int input = std::stoi(argv[i + 1]);
            if(input <= 0) {
                std::cerr << "Invalid interval duration" << std::endl;
                usage();
                exit(1);
            }

            util::PcapTest::test_interval = std::chrono::seconds(input);
            LOG_INFO("Interval setting : %u", util::PcapTest::test_interval);
        } else if(argv[i] == std::string("--help") ||
                  argv[i] == std::string("-h")) {
            usage();
            exit(1);
        }
        i++;
    }

    if(util::PcapTest::if_name.length() == 0) {
        usage();
        exit(1);
    }

    return RUN_ALL_TESTS();
}
