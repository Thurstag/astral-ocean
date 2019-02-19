// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "logger.h"

#include <iostream>

#include <boost/core/demangle.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

bool ao::core::Logger::initiated_ = false;

ao::core::Logger::Logger(std::type_info const& _class) : data(ao::core::LoggerInstance::Data(boost::core::demangle(_class.name()))) {}

void ao::core::Logger::Init() {
    // Check status
    if (ao::core::Logger::initiated_) {
        return;
    }

    // TODO: Add modifications

    // Add Severity in log
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    // Console
    boost::log::add_console_log(std::cout, boost::log::keywords::format = ao::core::logger::DefaultFormat, boost::log::keywords::auto_flush = true);

    // Set minimum level
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);

    // Add common attributes
    boost::log::add_common_attributes();
    ao::core::Logger::initiated_ = true;
}

bool ao::core::Logger::Initiated() {
    return ao::core::Logger::initiated_;
}

ao::core::LoggerInstance ao::core::Logger::operator<<(ao::core::Logger::Level level) const {
    return ao::core::LoggerInstance(level, this->data);
}

void ao::core::Logger::SetMinLevel(ao::core::Logger::Level level) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}
