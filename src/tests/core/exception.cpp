// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <sstream>

#include <ao/core/exception/expiration_exception.hpp>
#include <ao/core/exception/index_out_of_range.h>
#include <boost/algorithm/string/predicate.hpp>
#include <ao/core/exception/exception.h>
#include <gtest/gtest.h>

namespace ao::test {
	TEST(Exception, Constructor) {
		core::Exception exception("Message");
		core::Exception _exception(std::runtime_error("Message 2").what());

		ASSERT_STREQ("Message", exception.what());
		ASSERT_STREQ("Message 2", _exception.what());
	}

	TEST(Exception, OSOperator) {
		std::stringstream exception;
		exception << core::Exception("Message");

		std::stringstream _exception;
		_exception << core::Exception(std::runtime_error("Message 2").what(), false);

		ASSERT_STRNE("Message", exception.str().c_str()) << "Exception should have a stack trace";
		ASSERT_STREQ("Message 2", _exception.str().c_str()) << "Exception shouldn't have a stack trace";
	}

	TEST(ExpirationException, Constructor) {
		ASSERT_TRUE(boost::algorithm::ends_with(core::ExpirationException<int>(std::make_shared<int>(3)).what(), "pointer is expired"));
	}

	TEST(IndexOutOfRangeException, Constructor) {
		ASSERT_STREQ("Index out of range", core::IndexOutOfRangeException().what());
		ASSERT_STREQ("Index out of range, range is: 0 -> 10", core::IndexOutOfRangeException(std::make_pair<u64, u64>(0, 10)).what());
	}
}
