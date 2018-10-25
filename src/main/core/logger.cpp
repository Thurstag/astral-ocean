#include "logger.h"

ao::core::Logger::Logger(std::type_info const& type) {
	std::vector<std::string> prefixes = { "class", "struct" };
	std::string name = boost::core::demangle(type.name());

	// Optimize name
	for (std::string prefix : prefixes) {
		if (boost::starts_with(name, prefix)) {
			name = boost::lexical_cast<std::string>(name.c_str() + prefix.size());
		}
	}
	boost::trim(name);

	// Create logger
	this->category = &log4cpp::Category::getInstance(name);

	// TODO: Add support for file import & modifications

	// Define layout
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d{%d/%m/%Y %H:%M:%S.%l} [Thread-%t] [%p] %c - %m\n");

	// Define appender
	log4cpp::Appender *appender = new log4cpp::OstreamAppender("console", &std::cout);
	appender->setLayout(layout);

	// Define priority & appenders
	this->category->setPriority(log4cpp::Priority::DEBUG);
	this->category->addAppender(appender);
}

ao::core::Logger::~Logger() {
	// this->category will be released by log4cpp lib
}

log4cpp::CategoryStream ao::core::Logger::operator<<(log4cpp::Priority::Value priority) {
	return this->category->getStream(priority);
}
