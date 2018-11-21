#pragma once

#include <ao/vulkan/engine/wrappers/buffers/array/basic_buffer.hpp>

namespace ao {
	namespace test {
		template<class T, size_t N>
		class TestBasicArrayBuffer : public vulkan::BasicArrayBuffer<T, N> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			TestBasicArrayBuffer(std::weak_ptr<vulkan::Device> device);

			/// <summary>
			/// Method to get mapper
			/// </summary>
			/// <returns>Mapper</returns>
			void* getMapper();
		};

		template<class T, size_t N>
		TestBasicArrayBuffer<T, N>::TestBasicArrayBuffer(std::weak_ptr<vulkan::Device> device) : vulkan::BasicArrayBuffer<T, N>(device) {}

		template<class T, size_t N>
		void * TestBasicArrayBuffer<T, N>::getMapper() {
			return this->mapper;
		}

		template<class T>
		class TestBasicDynamicArrayBuffer : public vulkan::BasicDynamicArrayBuffer<T> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="count">Count</param>
			/// <param name="device">Device</param>
			TestBasicDynamicArrayBuffer(size_t count, std::weak_ptr<vulkan::Device> device);

			/// <summary>
			/// Method to get mapper
			/// </summary>
			/// <returns>Mapper</returns>
			void* getMapper();
		};

		template<class T>
		TestBasicDynamicArrayBuffer<T>::TestBasicDynamicArrayBuffer(size_t count, std::weak_ptr<vulkan::Device> device) : vulkan::BasicDynamicArrayBuffer<T>(count, device) {}

		template<class T>
		void * TestBasicDynamicArrayBuffer<T>::getMapper() {
			return this->mapper;
		}
	}
}
