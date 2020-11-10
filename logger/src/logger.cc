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

#include <cstdarg>
#include <string>
#include <iostream>
#include <fstream>

#include "logger.hpp"

namespace util {

void Logger::registerLogger(ILogger::Ptr logger) {
    loggers_.push_back(logger);
}

void Logger::out(LogLevel level, const char* format, ...) noexcept {
    if(level < defaultLevel_) {
        return;
    }

    char buf[BUF_SIZE];
    va_list args;

    va_start(args, format);
    vsnprintf(buf, BUF_SIZE, format, args);
    va_end(args);

    for (auto& logger : loggers_) {
        logger->out(level, buf);
    }
}

void Logger::out(LogLevel level, const std::string &format, ...) noexcept {
    if(level < defaultLevel_) {
        return;
    }

    char buf[BUF_SIZE];
    va_list args;

    va_start(args, format);
    vsnprintf(buf, BUF_SIZE, format.c_str(), args);
    va_end(args);

    for (auto& logger : loggers_) {
        logger->out(level, buf);
    }
}

void Logger::setLogLevel(LogLevel level) {
    defaultLevel_ = level;
}

void Logger::setLocale(int category) {
    std::setlocale(category, "");
}

#ifdef DLT_ENABLED
DltLogger::DltLogger(const char* app_name, const char* app_desc, const char* context_name, const char* context_desc) {
    DLT_REGISTER_APP(app_name, app_desc);
    DLT_REGISTER_CONTEXT(context_, context_name, context_desc);

    DLT_ENABLE_LOCAL_PRINT();
}

DltLogger::~DltLogger() {
    DLT_UNREGISTER_CONTEXT(context_);
    DLT_UNREGISTER_APP();
}

void DltLogger::out(LogLevel level, const char* str) {
    DltLogLevelType type[] = {DLT_LOG_DEBUG, DLT_LOG_VERBOSE, DLT_LOG_INFO, DLT_LOG_WARN, DLT_LOG_ERROR, DLT_LOG_FATAL};

    DLT_LOG(context_, type[(uint8_t)level], DLT_STRING(str));
}
#endif //DLT_ENABLED

OutStrmLogger::OutStrmLogger() : out_(std::make_shared<std::ostream>(std::cout.rdbuf())) {}

OutStrmLogger::OutStrmLogger(const std::string fileName) : fout_(std::make_shared<std::ofstream>()) {
    try {
        fout_->open(fileName);
        out_ = std::make_shared<std::ostream>(fout_->rdbuf());
    } catch(std::ofstream::failure const &e) {
        std::cerr << "Fail to create out stream : " << e.what() << std::endl;
    }
}

OutStrmLogger::~OutStrmLogger() {
    if(out_.get()) {
        out_->flush();
    }
    if(fout_.get()) {
        fout_->close();
    }
}

void OutStrmLogger::out(LogLevel level, const char* str) {
    if(out_.get()) {
        *out_ << logLevelStr_[static_cast<int>(level)] << str << std::endl;
    }
}

} //namespace util
