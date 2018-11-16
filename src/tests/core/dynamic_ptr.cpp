#include <sstream>

#include <ao/core/memory/dynamic_ptr.hpp>
#include <ao/core/exception/exception.h>
#include <gtest/gtest.h>

namespace ao {
	namespace test {
		class Object {
		public:
			Object() = default;
			Object(int _i) : i(_i) {};
			virtual ~Object() = default;

			void increment() { this->i++; }
			int value() { return i; }

		private:
			int i = 0;
		};

		void incrementWithAssert(core::dynamic_ptr<Object> ptr, int i) {
			ASSERT_EQ(i, ptr->value());
			ASSERT_EQ(2, ptr.use_count());

			ptr->increment();
		}

		TEST(DynamicPointer, DefaultConstructor) {
			core::dynamic_ptr<Object> ptr;

			try {
				ptr->increment();
			} catch (core::Exception& e) {
				ASSERT_STREQ("Pointer isn't initialized", e.what());
			} catch (...) {
				FAIL() << "Except: Pointer isn't initialized";
			}
		}

		TEST(DynamicPointer, MakeDynamic) {
			core::dynamic_ptr<Object> ptr = core::make_dynamic<Object>(1);

			ASSERT_EQ(1, ptr->value());
			ASSERT_EQ(1, ptr.use_count());
		}

		TEST(DynamicPointer, UseCount) {
			core::dynamic_ptr<Object> ptr = core::make_dynamic<Object>(1);
			core::dynamic_ptr<Object> ptr2 = core::make_dynamic<Object>(1);

			ASSERT_EQ(1, ptr.use_count());
			ASSERT_EQ(1, ptr.use_count());

			ptr2 = ptr;
			ASSERT_EQ(2, ptr.use_count());
		}

		TEST(DynamicPointer, Update) {
			core::dynamic_ptr<Object> ptr, ptr2, ptr3;
			ptr = ptr2 = core::make_dynamic<Object>(1);

			ASSERT_EQ(2, ptr.use_count());
			ASSERT_EQ(1, ptr->value());
			ASSERT_EQ(1, ptr->value());

			core::update(ptr, 2);
			ASSERT_EQ(2, ptr.use_count());
			ASSERT_EQ(2, ptr->value());
			ASSERT_EQ(2, ptr->value());

			core::update(ptr3, 4);
			ASSERT_EQ(1, ptr3.use_count());
			ASSERT_EQ(4, ptr3->value());
		}

		TEST(DynamicPointer, PassInArgument) {
			core::dynamic_ptr<Object> ptr = core::make_dynamic<Object>(1);

			ASSERT_EQ(1, ptr.use_count());
			ASSERT_EQ(1, ptr->value());

			incrementWithAssert(ptr, 1);
			ASSERT_EQ(2, ptr->value());
		}
	}
}
