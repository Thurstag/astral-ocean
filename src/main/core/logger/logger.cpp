#include "logger.h"

std::map<std::string, ao::core::Logger> ao::core::Logger::instances = std::map<std::string, ao::core::Logger>();
log4cpp::Priority::Value ao::core::Logger::level = LogLevel::DEBUG;

ao::core::Logger::Logger(std::type_info const& type) {
	this->category = &log4cpp::Category::getInstance(boost::core::demangle(type.name()));

	// TODO: Add support for file import & modifications

	// Define layout
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d{%d/%m/%Y %H:%M:%S.%l} [Thread-%t] [%p] %c - %m\n");

	// Define appender
	log4cpp::Appender *appender = new log4cpp::OstreamAppender("console", &std::cout);
	appender->setLayout(layout);

	// Define priority & appenders
	this->category->setPriority(ao::core::Logger::level);
	this->category->addAppender(appender);
}

log4cpp::CategoryStream ao::core::Logger::operator<<(log4cpp::Priority::Value priority) {
	return this->category->getStream(priority);
}

void ao::core::Logger::SetMinLevel(log4cpp::Priority::Value level) {
	ao::core::Logger::level = level;
	ao::core::Logger::Update([level](ao::core::Logger& logger) { logger.category->setPriority(level); });
}

void ao::core::Logger::Update(std::function<void(Logger&)> function) {
	for (auto& pair : ao::core::Logger::instances) {
		function(pair.second);
	}
}
