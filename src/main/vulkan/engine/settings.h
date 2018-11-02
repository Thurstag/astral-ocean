#pragma once

#include <string>

namespace ao {
	namespace vulkan {
		struct WindowSettings {
			std::string name;

			uint64_t width;
			uint64_t height;

			bool rezisable;
			bool vsync;

			WindowSettings() = default;
			WindowSettings(std::string name, uint64_t width, uint64_t height, bool rezisable = false, bool vsync = false) {
				this->name = name;
				this->width = width;
				this->height = height;
				this->rezisable = rezisable;
				this->vsync = vsync;
			}
		};

		struct EngineSettings {
			/* WINDOW SECTION */
			WindowSettings window;

			EngineSettings() = default;
			EngineSettings(WindowSettings window) {
				this->window = window;
			}
		};
	}
}
