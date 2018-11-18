#pragma once

#include <ao/core/logger/logger.h>

#include "../device.h"

namespace ao {
	namespace vulkan {
		template<class T>
		class Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_device">Device</param>
			Buffer(std::weak_ptr<Device> _device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Buffer() = default;

			/// <summary>
			/// Method to know if object has a buffer
			/// </summary>
			/// <returns>True or False</returns>
			virtual bool hasBuffer() = 0;

			/// <summary>
			/// Method to update buffer
			/// </summary>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual Buffer<T>& update(T data) = 0;

			/// <summary>
			/// Method to get buffer
			/// </summary>
			/// <returns>vk::Buffer</returns>
			virtual vk::Buffer& buffer() = 0;

			/// <summary>
			/// Method to get buffer's size
			/// </summary>
			/// <returns>Size</returns>
			virtual vk::DeviceSize size() = 0;

		protected:
			core::Logger LOGGER = core::Logger::getInstance<Buffer<T>>();
			std::weak_ptr<Device> device;
		};

		/* IMPLEMENTATION */

		template<class T>
		Buffer<T>::Buffer(std::weak_ptr<Device> _device) : device(_device) {}
	}
}
