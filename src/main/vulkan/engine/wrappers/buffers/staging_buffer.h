#pragma once

#include "buffer.h"

namespace ao {
	namespace vulkan {
		class StagingBuffer : public Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="_usage">Usage</param>
			/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
			StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool _memoryBarrier = false);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~StagingBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

			/// <summary>
			/// Method to get offset of fragment at index
			/// </summary>
			/// <param name="index">Fragment index</param>
			/// <returns>Offset</returns>
			virtual vk::DeviceSize offset(size_t index);

			virtual Buffer* map() override;

			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			bool hasBuffer() override;

		protected:
			vk::CommandBufferUsageFlags usage;
			bool memoryBarrier;

			std::shared_ptr<Buffer> deviceBuffer;
			std::shared_ptr<Buffer> hostBuffer;

			vk::CommandBuffer commandBuffer;
			vk::Fence fence;

			/// <summary>
			/// Method to synchronize host & device memories
			/// </summary>
			void sync();
		};
	}
}
