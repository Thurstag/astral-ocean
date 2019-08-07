// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "log.h"

#include <iostream>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

boost::log::sources::severity_logger<boost::log::trivial::severity_level> ao::core::Logger::logger;

void ao::core::Logger::Init() {
    // Console logging
    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = (boost::log::expressions::stream
                                        << "["
                                        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                                        << "] [Thread-"
                                        << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "] ["
                                        << boost::log::trivial::severity << "] " << boost::log::expressions::smessage),
        boost::log::keywords::auto_flush = true);

    // Set minimum level
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);

    // Add common attributes
    boost::log::add_common_attributes();
}
