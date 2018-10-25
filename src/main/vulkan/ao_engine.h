#pragma once

#include <boost/throw_exception.hpp>
#include <vulkan/vulkan.h>
#include <core/logger.h>

#include "engine_settings.h"
#include "vk_utilities.h"
#include "ao_device.h"

namespace ao {
	namespace vk {
		/// <summary>
		/// AOEngine class
		/// </summary>
		class AOEngine {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="settings">Settings</param>
			AOEngine(EngineSettings settings);
			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~AOEngine();

			/// <summary>
			/// Methdo to run engine
			/// </summary>
			virtual void run();
		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AOEngine>();
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

			/// <summary>
			/// Method to select a VkPhysicalDevice
			/// </summary>
			/// <param name="devices">VkPhysicalDevices</param>
			/// <returns>Index</returns>
			virtual uint64_t selectVkPhysicalDevice(std::vector<VkPhysicalDevice>& devices);

		private:
			VkInstance instance;
			AODevice device;
		};
	}
}
