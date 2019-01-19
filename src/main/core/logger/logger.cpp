// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "logger.h"

ao::core::Logger::Logger(std::type_info const& type) {
	this->key = boost::core::demangle(type.name());
}

void ao::core::Logger::Init() {
	// TODO: Add modifications

	// Add Severity in log
	boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

	// Console
	boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%] [Thread-%ThreadID%] [%Severity%] %Message%", boost::log::keywords::auto_flush = true);

	// Set minimum level
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);

	// Add common attributes
	boost::log::add_common_attributes();
}

ao::core::ProxyLogger ao::core::Logger::operator<<(LogLevel level) const {
	return ao::core::ProxyLogger(level, ao::core::LoggerData(this->key));
}

void ao::core::Logger::SetMinLevel(ao::core::LogLevel level) {
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
}
