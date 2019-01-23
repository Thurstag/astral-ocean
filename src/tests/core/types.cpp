// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/types.h>
#include <gtest/gtest.h>

namespace ao::test {
    TEST(Integers, SizeOf) {
        ASSERT_EQ(sizeof(u8), sizeof(std::uint8_t));
        ASSERT_EQ(sizeof(u16), sizeof(std::uint16_t));
        ASSERT_EQ(sizeof(u32), sizeof(std::uint32_t));
        ASSERT_EQ(sizeof(u64), sizeof(std::uint64_t));

        ASSERT_EQ(sizeof(s8), sizeof(std::int8_t));
        ASSERT_EQ(sizeof(s16), sizeof(std::int16_t));
        ASSERT_EQ(sizeof(s32), sizeof(std::int32_t));
        ASSERT_EQ(sizeof(s64), sizeof(std::int64_t));
    }
}  // namespace ao::test
