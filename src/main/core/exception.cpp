#include "exception.h"

ao::core::Exception::Exception(std::string message) : std::exception(message.c_str()) {
	// Init stack trace
	this->stack = boost::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
}

ao::core::Exception::Exception(std::exception & e) : ao::core::Exception(e.what()) {
	this->stack.reset();
}

ao::core::Exception::~Exception() {

}

std::ostream & ao::core::operator<<(std::ostream & os, const Exception & e) {
	// Display message
	os << e.what();

	// Display stack trace
	if (e.stack) {
		std::stringstream _stack;
		_stack << *e.stack;
		std::string stack = _stack.str();

		os << std::endl << stack.substr(0, stack.size() - 1);
	}
	return os;
}
