#include <gtest/gtest.h>

#include "../helpers/vk_instance.hpp"
#include "../helpers/vk_buffers.hpp"
#include "../helpers/tests.h"

namespace ao {
	namespace test {
		struct Object {
			u64 i;

			explicit Object(u64 _i) : i(_i) {}
		};

		TEST(BasicBuffer, NoInit) {
	        // 'Mute' logger
			core::Logger::SetMinLevel(log4cpp::Priority::FATAL);

			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			vulkan::BasicArrayBuffer<Object, 2> b(instance.device);
			vulkan::BasicDynamicArrayBuffer<Object> b2(2, instance.device);
		}

		TEST(BasicBuffer, Init) {
			// 'Mute' logger
			core::Logger::SetMinLevel(log4cpp::Priority::FATAL);

			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			vulkan::BasicArrayBuffer<Object, 2> b(instance.device);
			vulkan::BasicDynamicArrayBuffer<Object> b2(2, instance.device);

			// Init
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));

			// Free old buffers and init others
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
		}

		TEST(BasicBuffer, Update) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));

			// Normal update
			b.update({ Object(1), Object(2) });
			std::vector<Object> v = { Object(1), Object(2) };
			b2.update(v);

			// Out of range update
			try {
				std::vector<Object> v = { Object(1), Object(2), Object(1), Object(2) };
				b2.update(v);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}

			// Check content
			Object* bMapper = static_cast<Object*>(b.getMapper());
			Object* b2Mapper = static_cast<Object*>(b2.getMapper());

			ASSERT_EQ(1, bMapper->i);
			ASSERT_EQ(2, (bMapper + 1)->i);
			ASSERT_EQ(1, b2Mapper->i);
			ASSERT_EQ(2, (b2Mapper + 1)->i);
		}

		TEST(BasicBuffer, UpdateButNotInit) {
	        // Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);

			try {
				b.update({ Object(1), Object(2) });
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
			try {
				std::vector<Object> v = { Object(1), Object(2) };
				b2.update(v);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
		}

		TEST(BasicBuffer, UpdateFragmentButNotInit) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);
			Object* o = new Object(4);

			// Not init
			try {
				b.updateFragment(1, o);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
			try {
				b2.updateFragment(1, o);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}

			delete o;
		}

		TEST(BasicBuffer, UpdateFragment) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));

			// Normal update
			b.update({ Object(1), Object(2) });
			std::vector<Object> v = { Object(1), Object(2) };
			b2.update(v);

			// Check content
			Object* bMapper = static_cast<Object*>(b.getMapper());
			Object* b2Mapper = static_cast<Object*>(b2.getMapper());

			ASSERT_EQ(1, bMapper->i);
			ASSERT_EQ(2, (bMapper + 1)->i);
			ASSERT_EQ(1, b2Mapper->i);
			ASSERT_EQ(2, (b2Mapper + 1)->i);

			// Fragment update
			Object* o = new Object(4);
			b.updateFragment(1, o);
			b2.updateFragment(1, o);

		    // Out of range update
			try {
				b.updateFragment(10, o);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
			try {
				b2.updateFragment(10, o);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}

			// Check content
			ASSERT_EQ(1, bMapper->i);
			ASSERT_EQ(4, (bMapper + 1)->i);
			ASSERT_EQ(1, b2Mapper->i);
			ASSERT_EQ(4, (b2Mapper + 1)->i);

			delete o;
		}

		TEST(BasicBuffer, Offset) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));

			// Normal update
			b.update({ Object(1), Object(2) });
			std::vector<Object> v = { Object(1), Object(2) };
			b2.update(v);

			// Get offsets
			Object* bMapper = (Object*)((u64)b.getMapper() + b.offset(1));
			Object* b2Mapper = (Object*)((u64)b2.getMapper() + b2.offset(1));

			// Check content
			ASSERT_EQ(2, bMapper->i);
			ASSERT_EQ(2, b2Mapper->i);

			// Out of range
			try {
				b.offset(10);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
			try {
				b2.offset(10);
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
		}

		TEST(BasicBuffer, Map) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicArrayBuffer<Object, 2> b = TestBasicArrayBuffer<Object, 2>(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));
			TestBasicDynamicArrayBuffer<Object> b2 = TestBasicDynamicArrayBuffer<Object>(2, instance.device);
			b2.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, sizeof(Object));

			// Map
			b.map();
			b2.map();

			// Already map
			try {
				b.map();
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
			try {
				b2.map();
				FAIL() << "Should throw a core::Exception";
			} catch (core::Exception e) {
			} catch (...) {
				FAIL() << "Should catch a core::Exception";
			}
		}
	}
}
