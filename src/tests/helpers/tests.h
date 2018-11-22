#pragma once

#include <functional>

#include <boost/optional.hpp>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
/// <summary>
/// Macro to skip a test if condition is true
/// </summary>
#define SKIP_TEST(condition, message) {                                     \
    if (condition) {					                                    \
		std::cout << "Skip test because: " << #message << std::endl;      	\
        return;                                                             \
	}                                                                       \
}

		/// <summary>
		/// Method to assert an exception
		/// </summary>				   
		/// <param name="code">Code that will throw an exception</param>
		/// <param name="assert">Function to assert exception catched</param>
		template<class T>
		inline void ASSERT_EXCEPTION(std::function<void()> code, boost::optional<std::function<void(T)>> assert = boost::none) {
			try {
				code();

				FAIL() << "Should throw a " << typeid(T).name();
			} catch (T e) {
				if (assert) {
					(*assert)(e);
				}
			} catch (...) {
				FAIL() << "Should catch a " << typeid(T).name();
			}
		}
	}
}
