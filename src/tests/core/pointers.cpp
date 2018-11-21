#include <ao/core/utilities/pointers.h>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
		struct Object {
			int i = 0;

			explicit Object(int _i) : i(_i) {}
		};

		TEST(WeakPointer, Empty) {
			std::weak_ptr<int> ptr(std::make_shared<int>());

			try {
				core::shared(ptr);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
		}

		TEST(WeakPointer, Fill) {
			std::shared_ptr<Object> shared = std::make_shared<Object>(4);
			std::weak_ptr<Object> weak(shared);
			
			if (auto ptr = core::shared(weak)) {
				ASSERT_EQ(4, ptr->i);
			}
		}
	}
}
