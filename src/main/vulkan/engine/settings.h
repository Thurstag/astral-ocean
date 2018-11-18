#pragma once

#include <string>
#include <thread>

namespace ao {
	namespace vulkan {
		struct WindowSettings {
			std::string name;

			u64 width;
			u64 height;

			bool rezisable = true;
			bool vsync = false;

			/// <summary>
			/// Constructor
			/// </summary>
			WindowSettings() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_name">Window's name</param>
			/// <param name="_width">Window's width</param>
			/// <param name="_height">Window's height</param>
			/// <param name="_rezisable">Window is rezisable</param>
			/// <param name="_vsync">V-Sync is enabled</param>
			explicit WindowSettings(std::string _name, u64 _width, u64 _height, bool _rezisable = true, bool _vsync = false) :
				name(_name), width(_width), height(_height), rezisable(_rezisable), vsync(_vsync) {}
		};

		struct CoreSettings {
			int threadPoolSize = std::thread::hardware_concurrency();
			bool validationLayers = false;

			/// <summary>
			/// Constructor
			/// </summary>
			CoreSettings() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_threadPoolSize">Thread pool's size</param>
			/// <param name="_validationLayers">Enable validation layers</param>
			explicit CoreSettings(int _threadPoolSize = std::thread::hardware_concurrency(), bool _validationLayers = false) :
				threadPoolSize(_threadPoolSize), validationLayers(_validationLayers) {}
		};

		struct EngineSettings {
			WindowSettings window;
			CoreSettings core;

			/// <summary>
			/// Constructor
			/// </summary>
			EngineSettings() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_window">Window settings</param>
			/// <param name="_core">Core settings</param>
			explicit EngineSettings(WindowSettings _window, CoreSettings _core) : window(_window), core(_core) {}
		};
	}
}
