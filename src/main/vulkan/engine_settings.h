#pragma once

#include <string>

namespace ao {
	namespace vk {
		struct WindowSettings {
			std::string name;

			uint64_t width;
			uint64_t height;

			bool rezisable = false;
		};

		struct EngineSettings {
			/* VULKAN SECTION */
			bool vkValidationLayers = false;

			/* WINDOW SECTION */
			WindowSettings window;
		};
	}
}
