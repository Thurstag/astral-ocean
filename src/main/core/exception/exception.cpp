// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "exception.h"

ao::core::Exception::Exception(std::string const& message, bool stacktrace) : std::runtime_error(message) {
    // Init stack trace
	if (stacktrace) {
		this->stack = std::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
	}
}

ao::core::Exception::Exception(char const * message, bool stacktrace) : std::runtime_error(message) {
	// Init stack trace
	if (stacktrace) {
		this->stack = std::make_optional(boost::stacktrace::stacktrace(3, static_cast<std::size_t>(-1)));
	}
} 
