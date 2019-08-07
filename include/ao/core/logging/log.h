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

        /**
         * @brief Define a logging attribute
         *
         * @tparam Type Attribute type
         * @param name Name
         * @param value Value
         * @return Type Value
         */
        template<typename Type>
        static Type SetLogAttribute(const char* name, Type value) {
            auto attr =
                boost::log::attribute_cast<boost::log::attributes::mutable_constant<Type>>(boost::log::core::get()->get_thread_attributes()[name]);

            attr.set(value);
            return attr.get();
        }
    };
}  // namespace ao::core

#define LOG_MSG(sev)                                                                                                                \
    BOOST_LOG_STREAM_WITH_PARAMS(                                                                                                   \
        ::ao::core::Logger::logger,                                                                                                 \
        (::ao::core::Logger::SetLogAttribute("File", std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1)))( \
            ::ao::core::Logger::SetLogAttribute("Line", __LINE__))(::boost::log::keywords::severity = (boost::log::trivial::sev)))