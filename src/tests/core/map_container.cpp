#include <ao/core/memory/map_container.hpp>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
		using Map = core::MapContainer<int, int, std::map<int, int>::iterator>;

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
				}
				else if (it.first == 1 && it.second != 2) {
					FAIL() << "Should retrieve pair: 1-2";
				}
			}
		}
	}
}
