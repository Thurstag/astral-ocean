#include "exception.h"

ao::core::Exception::Exception(std::string message, bool stacktrace) : std::runtime_error(message) {
    // Init stack trace
	if (stacktrace) {
		this->stack = boost::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
	}
}

ao::core::Exception::Exception(char const * message, bool stacktrace) : std::runtime_error(message) {
	// Init stack trace
	if (stacktrace) {
		this->stack = boost::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
	}
}

ao::core::Exception::~Exception() {

} 
