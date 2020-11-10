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

#ifndef DATE_UTIL_HPP__
#define DATE_UTIL_HPP__

#include <ctime>

#include "logger.hpp"

namespace util {

enum class TIME_FORMAT {
    YYMMDD_HHMMSS = 0, //YearMonthDay_HourMinuteSecond
    YYMMDD, //YearMonthDay
    HHMMSS, //HourMinuteSecond
    LENGTH_
};

class DateUtil {
public:
    static std::string getCurrentTimeStr(TIME_FORMAT format = TIME_FORMAT::YYMMDD_HHMMSS) {
        if(format >= TIME_FORMAT::LENGTH_) {
            LOG_WARN("Invalid TIME_FORMAT. Set default [" + timeFormats[0] + "]");
            format = TIME_FORMAT::YYMMDD_HHMMSS;
        }

        time_t now;
        struct tm * timeinfo;
        char buffer[80];

        time (&now);
        timeinfo = localtime(&now);

        strftime(buffer, sizeof(buffer), timeFormats[static_cast<int>(format)].c_str(), timeinfo);

        return std::string(buffer);
    }

    static const std::string timeFormats[static_cast<int>(TIME_FORMAT::LENGTH_)];
};

const std::string DateUtil::timeFormats[static_cast<int>(TIME_FORMAT::LENGTH_)] {"%Y%m%d-%H%M%S", "%Y%m%d", "%H%M%S"};

} //namespace util

#endif //DATE_UTIL_HPP__
