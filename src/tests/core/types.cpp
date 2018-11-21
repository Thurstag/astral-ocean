#include <ao/core/utilities/types.h>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
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
	}
}
