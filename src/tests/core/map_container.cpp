// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <gtest/gtest.h>
#include <ao/core/memory/map_container.hpp>

namespace ao::test {
    using Map = core::MapContainer<int, int>;

    TEST(MapContainer, AccessAndAssignOperator) {
        Map m;

        m[0] = 1;
        ASSERT_EQ(1, m[0]);

        ASSERT_EQ(0, m[10]);
    }

    TEST(MapContainer, Remove) {
        Map m;

        m[0] = 1;
        ASSERT_TRUE(m.remove(0));
        ASSERT_EQ(0, m.size());

        ASSERT_FALSE(m.remove(10));
    }

    TEST(MapContainer, Exists) {
        Map m;
        m[0] = 1;

        ASSERT_TRUE(m.exists(0));
        ASSERT_FALSE(m.exists(10));
    }

    TEST(MapContainer, Clear) {
        Map m;
        m[0] = 1;

        ASSERT_EQ(1, m.size());
        m.clear();
        ASSERT_EQ(0, m.size());
    }

    TEST(MapContainer, Iterator) {
        Map m;
        m[0] = 1;
        m[1] = 2;

        for (auto it : m) {
            if (it.first == 0 && it.second != 1) {
                FAIL() << "Should retrieve pair: 0-1";
            } else if (it.first == 1 && it.second != 2) {
                FAIL() << "Should retrieve pair: 1-2";
            }
        }
    }
}  // namespace ao::test
