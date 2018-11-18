#include <sstream>

#include <ao/core/exception/expiration_exception.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <ao/core/exception/exception.h>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
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

			ASSERT_STRNE(exception.str().c_str(), "Message") << "Exception should have a stack trace";
			ASSERT_STREQ(_exception.str().c_str(), "Message 2") << "Exception shouldn't have a stack trace";
		}

		TEST(ExpirationException, Constructor) {
			ASSERT_TRUE(boost::algorithm::ends_with(core::ExpirationException<int>(std::make_shared<int>(3)).what(), "pointer is expired"));
		}
	}
}
