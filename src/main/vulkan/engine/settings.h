#pragma once

#include <string>
#include <thread>

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

			int threadPoolSize = std::thread::hardware_concurrency();

			EngineSettings() = default;
			EngineSettings(WindowSettings window, int threadPoolSize = std::thread::hardware_concurrency()) {
				this->window = window;
				this->threadPoolSize = threadPoolSize;
			}
		};
	}
}
