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

#include "logger.hpp"

/**
 * This example show how to print log messages at stdout
 */
int main(int argc, char **argv) {
    util::Logger::getInstance().registerLogger(std::make_shared<util::OutStrmLogger>());

    LOG_INFO("This message will print at stdout.");

    LOG_DEBUG("This message cannot be printed. Default log level is Verbose");

    util::Logger::getInstance().setLogLevel(util::LogLevel::Debug);

    LOG_DEBUG("You can see this message at stdout :D");

    return 0;
}