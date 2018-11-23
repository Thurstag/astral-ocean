#include <gtest/gtest.h>

#include "../helpers/vk_tuple_buffers.hpp"
#include "../helpers/vk_instance.hpp"
#include "../helpers/tests.h"

namespace ao {
	namespace test {
		struct Object {
			u64 i;

			explicit Object(u64 _i) : i(_i) {}
		};

		struct SecondObject {
			bool b;

			explicit SecondObject(bool _b) : b(_b) {}
		};

		/* BASIC */

		TEST(BasicBuffer, NotInit) {
			// 'Mute' logger
			core::Logger::Init();
			core::Logger::SetMinLevel(core::LogLevel::fatal);

			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			vulkan::BasicTupleBuffer<Object, SecondObject> b(instance.device);
		}

		TEST(BasicBuffer, Init) {

			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);

			// Init
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			// Free old buffers and init others
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			// Wrong size
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object) });
			});
		}

		TEST(BasicBuffer, Update) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Check content
			Object* oMapper = static_cast<Object*>(b.mapper(0));
			SecondObject* sMapper = static_cast<SecondObject*>(b.mapper(1));

			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			delete o;
			delete sO;
		}

		TEST(BasicBuffer, UpdateButNotInit) {
	        // Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.update(o, sO);
			});

			delete o;
			delete sO;
		}

		TEST(BasicBuffer, UpdateFragmentButNotInit) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);
			Object* o = new Object(4);

			// Not init
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.updateFragment(1, o);
			});
			delete o;
		}

		TEST(BasicBuffer, UpdateFragment) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Check content
			Object* oMapper = static_cast<Object*>(b.mapper(0));
			SecondObject* sMapper = static_cast<SecondObject*>(b.mapper(1));

			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			// Fragment update
			Object* o2 = new Object(4);
			b.updateFragment(0, o2);

		    // Out of range update
			ASSERT_EXCEPTION<core::IndexOutOfRangeException>([&]() {
				b.updateFragment(10, o2);
			});

			// Check content
			ASSERT_EQ(4, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			delete o;
			delete o2;
			delete sO;
		}

		TEST(BasicBuffer, Offset) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Get offsets
			Object* oMapper = (Object*)((u64)b.mapper(0) + b.offset(0));
			SecondObject* sMapper = (SecondObject*)((u64)b.mapper(0) + b.offset(1));

			// Check content
			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			// Out of range
			ASSERT_EXCEPTION<core::IndexOutOfRangeException>([&]() {
				b.offset(10);
			});

			delete o;
			delete sO;
		}

		TEST(BasicBuffer, Map) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestBasicTupleBuffer<Object, SecondObject> b(instance.device);
			b.init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible, { sizeof(Object), sizeof(SecondObject) });

			// Map
			b.map();

			// Already map
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.map();
			});
		}

		/* STAGING */

		TEST(StagingBuffer, NotInit) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			vulkan::StagingTupleBuffer<Object, SecondObject> b(instance.device);
		}

		TEST(StagingBuffer, Init) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			vulkan::StagingTupleBuffer<Object, SecondObject> b(instance.device);

			// Init
			b.init({ sizeof(Object), sizeof(SecondObject) });

			// Free old buffers and init others
			b.init({ sizeof(Object), sizeof(SecondObject) });
		}

		TEST(StagingBuffer, Update) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			b.init({ sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Check content
			Object* oMapper = static_cast<Object*>(b.mapper(0));
			SecondObject* sMapper = static_cast<SecondObject*>(b.mapper(1));

			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			delete o;
			delete sO;
		}

		TEST(StagingBuffer, UpdateButNotInit) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Not init
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.update(o, sO);
			});

			delete o;
			delete sO;
		}

		TEST(StagingBuffer, UpdateFragmentButNotInit) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			Object* o = new Object(4);

			// Not init
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.updateFragment(0, o);
			});

			delete o;
		}

		TEST(StagingBuffer, UpdateFragment) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			b.init({ sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Check content
			Object* oMapper = static_cast<Object*>(b.mapper(0));
			SecondObject* sMapper = static_cast<SecondObject*>(b.mapper(1));

			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			// Fragment update
			Object* o2 = new Object(4);
			b.updateFragment(0, o2);

			// Out of range update
			ASSERT_EXCEPTION<core::IndexOutOfRangeException>([&]() {
				b.updateFragment(10, o2);
			});

			// Check content
			ASSERT_EQ(4, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			delete o;
			delete o2;
			delete sO;
		}

		TEST(StagingBuffer, Offset) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			b.init({ sizeof(Object), sizeof(SecondObject) });

			Object* o = new Object(1);
			SecondObject* sO = new SecondObject(true);

			// Normal update
			b.update(o, sO);

			// Get offsets
			Object* oMapper = (Object*)((u64)b.mapper(0) + b.offset(0));
			SecondObject* sMapper = (SecondObject*)((u64)b.mapper(0) + b.offset(1));

			// Check content
			ASSERT_EQ(1, oMapper->i);
			ASSERT_EQ(true, sMapper->b);

			// Out of range
			ASSERT_EXCEPTION<core::IndexOutOfRangeException>([&]() {
				b.offset(10);
			});

			delete o;
			delete sO;
		}

		TEST(StagingBuffer, Map) {
			// Init instance
			VkInstance instance;
			SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

			TestStagingTupleBuffer<Object, SecondObject> b(instance.device);
			b.init({ sizeof(Object), sizeof(SecondObject) });

			// Map
			b.map();

			// Already map
			ASSERT_EXCEPTION<core::Exception>([&]() {
				b.map();
			});
		}
	}
}
