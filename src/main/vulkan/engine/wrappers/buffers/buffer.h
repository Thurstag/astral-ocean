#pragma once

#include <memory>

#include <ao/core/utilities/pointers.h>
#include <ao/core/logger/logger.h>

#include "../device.h"

namespace ao {
	namespace vulkan {
		class Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			Buffer(std::weak_ptr<Device> _device);

			/// <summary>
			/// Destructor
			/// </summary>
			~Buffer() = default;

			/// <summary>
			/// Method to know if object has a buffer
			/// </summary>
			/// <returns>True or False</returns>
			virtual bool hasBuffer() = 0;

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
			core::Logger LOGGER = core::Logger::getInstance<Buffer>();
			std::weak_ptr<Device> device;
		};
	}
}
