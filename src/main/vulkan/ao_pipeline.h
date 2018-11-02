#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "ao_swapchain.h"
#include "ao_device.h"

namespace ao {
	namespace vulkan {
		struct AOPipeline {
		public:
			/* FIELDS */
			
			vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

			vk::PipelineLayout layout;
			vk::PipelineCache cache;
			vk::Pipeline pipeline;

			/* CON/DESTRUCTORS */

			/// <summary>
			/// Constructor
			/// </summary>
			AOPipeline() = default;
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			AOPipeline(AODevice* device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~AOPipeline();
		private:
			AODevice* device;
		};
	}
}
