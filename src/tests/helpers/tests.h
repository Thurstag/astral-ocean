// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>
#include <optional>

#include <gtest/gtest.h>

namespace ao::test {
/// <summary>
/// Macro to skip a test if condition is true
/// </summary>
#define SKIP_TEST(condition, message)                                    \
    {                                                                    \
        if (condition) {                                                 \
            std::cout << "Skip test because: " << #message << std::endl; \
            return;                                                      \
        }                                                                \
    }

    /// <summary>
    /// Method to assert an exception
    /// </summary>
    /// <param name="code">Code that will throw an exception</param>
    /// <param name="assert">Function to assert exception catched</param>
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
