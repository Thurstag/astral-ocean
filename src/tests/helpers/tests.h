// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>

#include <ao/core/utilities/optional.h>
#include <gtest/gtest.h>

namespace ao::test {
/**
 * @brief Macro to skip a text
 *
 * @param condition Condition
 * @param message Message displayed if condition is true
 *
 */
#define SKIP_TEST(condition, message)                                    \
    {                                                                    \
        if (condition) {                                                 \
            std::cout << "Skip test because: " << #message << std::endl; \
            return;                                                      \
        }                                                                \
    }

    /**
     * @brief Assert an exception
     *
     * @tparam T Exception type
     * @param code Code to execute
     * @param assert Assert on exception
     */
    template<class T>
    inline void ASSERT_EXCEPTION(std::function<void()> code, std::function<void(T)> assert = [](T e) {}) {
        try {
            code();

            FAIL() << "Should throw a " << typeid(T).name();
        } catch (T& e) {
            (assert)(e);
        } catch (...) {
            FAIL() << "Should catch a " << typeid(T).name();
        }
    }
}  // namespace ao::test
