#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "swapchain.h"
#include "device.h"

namespace ao {
	namespace vulkan {
		/// <summary>
		/// Wrapper for vulkan pipelines
		/// </summary>
		struct Pipeline {
		public:			
			vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

			vk::PipelineLayout layout;
			vk::PipelineCache cache;
			vk::Pipeline graphics;

			/// <summary>
			/// Constructor
			/// </summary>
			Pipeline() = default;
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			Pipeline(Device* device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Pipeline();
		private:
			Device* device;
		};
	}
}
