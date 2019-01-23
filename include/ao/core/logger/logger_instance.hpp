// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <sstream>
#include <string>

#include <fmt/format.h>
#include <boost/log/trivial.hpp>

#include "../exception/exception.h"

namespace ao::core {
    /// <summary>
    /// LoggerInstance class
    /// </summary>
    class LoggerInstance {
       public:
        /// <summary>
        /// Data class
        /// </summary>
        struct Data {
            std::string class_;

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="_class">Class</param>
            explicit Data(std::string const& _class) : class_(_class) {}
        };

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="_level">Level</param>
        /// <param name="_data">Data</param>
        explicit LoggerInstance(boost::log::trivial::severity_level const& _level, Data const& _data) : level(_level), data(_data) {}

        /// <summary>
        /// Destructor
        /// </summary>
        ~LoggerInstance() = default;

        /// <summary>
        /// Operator <<
        /// </summary>
        /// <param name="object">Object</param>
        template<class T>
        void operator<<(T const& object) const;

       protected:
        boost::log::trivial::severity_level level;
        Data data;
    };

    template<class T>
    void ao::core::LoggerInstance::operator<<(T const& object) const {
        // To String
        std::stringstream s;
        s << object;

        // Add more info
        std::string _message = fmt::format("{0} - {1}", this->data.class_, s.str());

        // Log
        switch (this->level) {
            case boost::log::trivial::trace:
                BOOST_LOG_TRIVIAL(trace) << _message;
                break;

            case boost::log::trivial::debug:
                BOOST_LOG_TRIVIAL(debug) << _message;
                break;

            case boost::log::trivial::info:
                BOOST_LOG_TRIVIAL(info) << _message;
                break;

            case boost::log::trivial::warning:
                BOOST_LOG_TRIVIAL(warning) << _message;
                break;

            case boost::log::trivial::error:
                BOOST_LOG_TRIVIAL(error) << _message;
                break;

            case boost::log::trivial::fatal:
                BOOST_LOG_TRIVIAL(fatal) << _message;
                break;

            default:
                throw ao::core::Exception(fmt::format("Unknown level: {0}", this->level));
        }
    }
}  // namespace ao::core
