// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace ao::core {
    /**
     * @brief Wrapper for Boost::log
     *
     */
    class Logger {
       public:
        static boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

        /**
         * @brief Init logger
         *
         */
        static void Init();
    };
}  // namespace ao::core

#define LOG_MSG(sev)                                                    \
    BOOST_LOG_SEV(::ao::core::Logger::logger, boost::log::trivial::sev) \
        << "[" << std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1) << ":" << __LINE__ << "] "