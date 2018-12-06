// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/pointers.h>
#include <gtest/gtest.h>

#include "../helpers/tests.h"

namespace ao::test {
	struct Object {
		int i = 0;

		explicit Object(int _i) : i(_i) {}
	};

	TEST(WeakPointer, Empty) {
		std::weak_ptr<int> ptr(std::make_shared<int>());

		ASSERT_EXCEPTION<core::Exception>([&]() {
			core::shared(ptr);
		});
	}

	TEST(WeakPointer, Fill) {
		std::shared_ptr<Object> shared = std::make_shared<Object>(4);
		std::weak_ptr<Object> weak(shared);

		if (auto ptr = core::shared(weak)) {
			ASSERT_EQ(4, ptr->i);
		}
	}
}
