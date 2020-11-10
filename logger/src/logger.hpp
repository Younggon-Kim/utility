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

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <memory>
#include <string>
#include <vector>
#ifdef DLT_ENABLED
#include <dlt/dlt.h>
#endif //DLT_ENABLED
#include <cstring> //strrchr

#include "singleton.hpp"

namespace util {

enum class LogLevel : uint8_t {
    Debug = 0,
    Verbose,
    Info,
    Warn,
    Error,
    Fatal
};

class ILogger {
public:
    using Ptr = std::shared_ptr<ILogger>;
    virtual ~ILogger() = default;
    virtual void out(LogLevel level, const char* str) = 0;
};

class Logger : public Singleton<Logger> {
public:
    void registerLogger(ILogger::Ptr logger);
    void out(LogLevel level, const char* format, ...) noexcept;
    void out(LogLevel level, const std::string &format, ...) noexcept;
    void setLogLevel(LogLevel level);
    void setLocale(int category);

private:
    friend class Singleton<Logger>;
    Logger() : defaultLevel_(LogLevel::Verbose) {}
    ~Logger() = default;
    void out_(LogLevel level, const char *format, ...);

    std::vector<ILogger::Ptr> loggers_;
    static constexpr uint64_t BUF_SIZE = 1024;
    LogLevel defaultLevel_;
};

#ifdef DLT_ENABLED
class DltLogger : public ILogger {
public:
    DltLogger(const char* app_name, const char* context_name, const char* app_desc = "app_description", const char* context_desc = "context_description");
    virtual ~DltLogger();
    virtual void out(LogLevel level, const char* str) override;

private:
    DltContext context_;
};
#endif //DLT_ENABLED

class OutStrmLogger : public ILogger {
public:
    OutStrmLogger();
    OutStrmLogger(const std::string fileName);
    ~OutStrmLogger();
    virtual void out(LogLevel level, const char* str) override;

private:
    std::shared_ptr<std::ostream> out_;
    std::shared_ptr<std::ofstream> fout_;
    const std::vector<std::string> logLevelStr_ {"[DEBUG]", "[VERBOSE]", "[INFO]", "[WARN]", "[ERROR]", "[FATAL]"};
};

} //namespace util

#define __FILENAME__ (std::strrchr(__FILE__, '/') ? (std::strrchr(__FILE__, '/') + 1) : __FILE__)

#ifdef LOG_ENABLED

#define LOG_FATAL(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Fatal, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

#define LOG_ERROR(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Error, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

#define LOG_WARN(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Warn, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

#define LOG_INFO(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Info, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

#define LOG_DEBUG(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Debug, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

#define LOG_VERBOSE(format, args...) \
    util::Logger::getInstance().out(util::LogLevel::Verbose, "[%s:%d][%s] " format, __FILENAME__, __LINE__, __func__, ##args)

//print message without file name, line, function name
#define LOG_DEBUG_RAW(format, args...)    util::Logger::getInstance().out(util::LogLevel::Debug, "" format, ##args)
#define LOG_VERBOSE_RAW(format, args...)  util::Logger::getInstance().out(util::LogLevel::Verbose, "" format, ##args)
#define LOG_INFO_RAW(format, args...)     util::Logger::getInstance().out(util::LogLevel::Info, "" format, ##args)
#define LOG_WARN_RAW(format, args...)     util::Logger::getInstance().out(util::LogLevel::Warn, "" format, ##args)
#define LOG_ERROR_RAW(format, args...)    util::Logger::getInstance().out(util::LogLevel::Error, "" format, ##args)
#define LOG_FATAL_RAW(format, args...)    util::Logger::getInstance().out(util::LogLevel::Fatal, "" format, ##args)

#else //LOG_ENABLED

#define LOG_FATAL(format, args...)
#define LOG_ERROR(format, args...)
#define LOG_WARN(format, args...)
#define LOG_INFO(format, args...)
#define LOG_DEBUG(format, args...)
#define LOG_VERBOSE(format, args...)

//print message without file name, line, function name
#define LOG_DEBUG_RAW(format, args...)
#define LOG_VERBOSE_RAW(format, args...)
#define LOG_INFO_RAW(format, args...)
#define LOG_WARN_RAW(format, args...)
#define LOG_ERROR_RAW(format, args...)
#define LOG_FATAL_RAW(format, args...)

#endif //LOG_ENABLED

#endif  //LOGGER_HPP_