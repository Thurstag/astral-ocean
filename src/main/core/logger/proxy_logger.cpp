#include "proxy_logger.h"

void ao::core::ProxyLogger::operator<<(std::string message) {
	this->log(message.c_str());
}

void ao::core::ProxyLogger::operator<<(const char * message) {
	this->log(message);
}

void ao::core::ProxyLogger::operator<<(core::Exception & exception) {
	this->log(exception.what());
}

void ao::core::ProxyLogger::log(const char * message) {
	// Add more info
	std::string _message = fmt::format("{0} - {1}", this->data.key, message);

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
