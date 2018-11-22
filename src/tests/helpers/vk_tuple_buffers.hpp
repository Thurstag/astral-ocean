#pragma once

#include <ao/vulkan/engine/wrappers/buffers/tuple/staging_buffer.hpp>
#include <ao/vulkan/engine/wrappers/buffers/tuple/basic_buffer.hpp>
#include <ao/vulkan/engine/wrappers/device.h>

namespace ao {
	namespace test {
		template<class ...T>
		class TestBasicTupleBuffer : public vulkan::BasicTupleBuffer<T...> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			TestBasicTupleBuffer(std::weak_ptr<vulkan::Device> device);

			/// <summary>
			/// Method to get mapper
			/// </summary>
			/// <param name="index">Index</param>
			/// <returns>Mapper</returns>
			void* mapper(size_t index);
		};

		template<class ...T>
		TestBasicTupleBuffer<T...>::TestBasicTupleBuffer(std::weak_ptr<vulkan::Device> device) : vulkan::BasicTupleBuffer<T...>(device) {}

		template<class ...T>
		void * TestBasicTupleBuffer<T...>::mapper(size_t index) {
			return this->fragments[index].second;
		}

		template<class ...T>
		class TestStagingTupleBuffer : public vulkan::StagingTupleBuffer<T...> {
		public:
	        /// <summary>
	        /// Constructor
	        /// </summary>
	        /// <param name="device">Device</param>
			TestStagingTupleBuffer(std::weak_ptr<vulkan::Device> device);

			/// <summary>
			/// Method to get mapper
			/// </summary>
			/// <param name="index">Index</param>
			/// <returns>Mapper</returns>
			void* mapper(size_t index);
		};

		template<class ...T>
		TestStagingTupleBuffer<T...>::TestStagingTupleBuffer(std::weak_ptr<vulkan::Device> device) :
			vulkan::StagingTupleBuffer<T...>(device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit),
			vulkan::TupleBuffer<T...>(device),
			vulkan::StagingBuffer(device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit) {}

		template<class ...T>
		void * TestStagingTupleBuffer<T...>::mapper(size_t index) {
			if (auto host = static_cast<TestBasicTupleBuffer<T...>*>(this->hostBuffer.get())) {
				return host->mapper(index);
			}
			throw core::Exception("Fail to get mapper");
		}
	}
}
