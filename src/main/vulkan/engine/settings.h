#pragma once

#include <string>
#include <thread>

namespace ao {
	namespace vulkan {
		struct WindowSettings {
			std::string name;

			u64 width;
			u64 height;

			bool rezisable;
			bool vsync;

			/// <summary>
			/// Constructor
			/// </summary>
			WindowSettings() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="name">Window's name</param>
			/// <param name="width">Window's width</param>
			/// <param name="height">Window's height</param>
			/// <param name="rezisable">Window is rezisable</param>
			/// <param name="vsync">V-Sync is enabled</param>
			WindowSettings(std::string name, u64 width, u64 height, bool rezisable = false, bool vsync = false) {
				this->name = name;
				this->width = width;
				this->height = height;
				this->rezisable = rezisable;
				this->vsync = vsync;
			}
		};

		struct EngineSettings {
			int threadPoolSize = std::thread::hardware_concurrency();
			WindowSettings window;

			/// <summary>
			/// Constructor
			/// </summary>
			EngineSettings() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="window">Window settings</param>
			/// <param name="threadPoolSize">Thread pool's size</param>
			EngineSettings(WindowSettings window, u32 threadPoolSize = std::thread::hardware_concurrency()) {
				this->window = window;
				this->threadPoolSize = threadPoolSize;
			}
		};
	}
}
