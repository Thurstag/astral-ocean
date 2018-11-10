#include "exception.h"

ao::core::Exception::Exception(std::string message) : std::runtime_error(message) {
	// Init stack trace
	this->stack = boost::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
}

ao::core::Exception::Exception(std::exception & e) : ao::core::Exception(e.what()) {
	this->stack.reset();
}

ao::core::Exception::~Exception() {

} 
