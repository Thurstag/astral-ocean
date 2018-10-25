#include "exception.h"

ao::core::Exception::Exception(std::string message) : std::exception(message.c_str()) {
	// Init stack trace
	this->stack = new boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1));
}

ao::core::Exception::Exception(std::exception & e) : ao::core::Exception(e.what()) {
	// Delete stack trace
	delete this->stack;
}

ao::core::Exception::~Exception() {
	if (this->stack != nullptr) {
		delete this->stack;
	}
}

std::ostream & ao::core::operator<<(std::ostream & os, const Exception & e) {
	std::stringstream _stack;
	_stack << *e.stack;
	std::string stack = _stack.str();

	os << e.what() << std::endl << stack.substr(0, stack.size() - 1);
	return os;
}