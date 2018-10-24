#pragma once

#include "boost/throw_exception.hpp"

#include "engine_settings.h"
#include "vulkan/vulkan.h"
#include "utilities.h"

namespace ao {
	namespace vk {
		/// <summary>
		/// Engine class
		/// </summary>
		class Engine {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="settings">Settings</param>
			Engine(EngineSettings settings);
			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Engine();

			/// <summary>
			/// Methdo to run engine
			/// </summary>
			virtual void run();
		protected:
			EngineSettings settings;

			/// <summary>
			/// Method to init vulkan
			/// </summary>
			void initVulkan();
			/// <summary>
			/// Method to clean-up vulkan
			/// </summary>
			void freeVulkan();

			/// <summary>
			/// Method to init window
			/// </summary>
			virtual void initWindow() = 0;
			/// <summary>
			/// Method to clean-up window
			/// </summary>
			virtual void freeWindow();

			/// <summary>
			/// Method to define main loop
			/// </summary>
			virtual void loop() = 0;

			/// <summary>
			/// Method to get vkInstance extensions
			/// </summary>
			/// <returns>Extensions</returns>
			virtual std::vector<char const*> instanceExtensions() = 0;

		private:
			VkInstance instance;
		};
	}
}
