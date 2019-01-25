// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/vulkan/engine/settings.h>
#include <gtest/gtest.h>

#include "../helpers/tests.h"

namespace ao::test {
    TEST(EngineSettings, Default) {
        vulkan::EngineSettings s;

        ASSERT_FALSE(s.exists("test"));
        ASSERT_TRUE(s.get<bool>("test", std::make_optional<bool>(true)));
        ASSERT_TRUE(s.exists("test"));
    }

    TEST(EngineSettings, Update) {
        vulkan::EngineSettings s;
        s.get<bool>("test") = false;

        ASSERT_FALSE(s.get<bool>("test"));

        s.get<bool>("test") = true;
        ASSERT_TRUE(s.get<bool>("test"));
    }

    TEST(EngineSettings, UpdateByRef) {
        vulkan::EngineSettings s;
        s.get<bool>("test") = false;

        bool& ref = s.get<bool>("test");
        ASSERT_FALSE(ref);
        ref = true;

        ASSERT_TRUE(s.get<bool>("test"));
    }

    TEST(EngineSettings, WrongCast) {
        // TODO: When logger is enhanced
    }
}  // namespace ao::test
