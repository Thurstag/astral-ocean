#pragma once

#include <sstream>

#include <ao/core/exception/exception.h>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
		TEST(Exception, Constructor) {
			core::Exception exception("Message");
			core::Exception _exception(std::exception("Message 2"));

			ASSERT_STREQ("Message", exception.what());
			ASSERT_STREQ("Message 2", _exception.what());
		}

		TEST(Exception, OSOperator) {
			std::stringstream exception;
			exception << core::Exception("Message");

			std::stringstream _exception;
			_exception << core::Exception(std::exception("Message 2"));

			ASSERT_STRNE(exception.str().c_str(), "Message") << "Exception should have a stack trace";
			ASSERT_STREQ(_exception.str().c_str(), "Message 2") << "Exception shouldn't have a stack trace";
		}
	}
}
