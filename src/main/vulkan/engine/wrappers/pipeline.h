#pragma once

#include <vector>

#include <ao/core/utilities/pointers.h>
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

			std::vector<vk::PipelineLayout> layouts;
			std::vector<vk::Pipeline> pipelines;
			vk::PipelineCache cache;

			/// <summary>
			/// Constructor
			/// </summary>
			Pipeline() = default;
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_device">Device</param>
			explicit Pipeline(std::weak_ptr<Device> _device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Pipeline();
		protected:
			std::weak_ptr<Device> device;
		};
	}
}
