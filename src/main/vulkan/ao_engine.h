#pragma once

#include <core/exception.h>
#include <vulkan/vulkan.h>
#include <core/logger.h>
#include <tuple>

#include "engine_settings.h"
#include "ao_swapchain.h"
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
			
			std::pair<VkSemaphore, VkSemaphore> semaphores; // First = Present semaphore & Second = Render semaphore

			EngineSettings settings;

			AOSwapChain* swapchain;
			VkInstance instance;
			AODevice* device;


			/// <summary>
			/// Method to init vulkan
			/// </summary>
			void initVulkan();
			/// <summary>
			/// Method to prepare vulkan before running
			/// </summary>
			void prepareVulkan();
			/// <summary>
			/// Method to clean-up vulkan
			/// </summary>
			void freeVulkan();

			/// <summary>
			/// Method to init surface
			/// </summary>
			/// <returns>Surface</returns>
			virtual VkSurfaceKHR& initSurface() = 0;
			/// <summary>
			/// Method to init window
			/// </summary>
			virtual void initWindow() = 0;
			/// <summary>
			/// Method to clean-up window
			/// </summary>
			virtual void freeWindow() = 0;

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
			/// Method to get device extensions that must be enabled on device
			/// </summary>
			/// <returns></returns>
			virtual std::vector<char const*> deviceExtensions();
			/// <summary>
			/// Method to define queue flags
			/// </summary>
			/// <returns>flags</returns>
			virtual VkQueueFlags queueFlags();
			/// <summary>
			/// Method to define command pool flags
			/// </summary>
			/// <returns>flags</returns>
			virtual VkCommandPoolCreateFlags commandPoolFlags();

			/// <summary>
			/// Method to select a VkPhysicalDevice
			/// </summary>
			/// <param name="devices">VkPhysicalDevices</param>
			/// <returns>Index</returns>
			virtual uint8_t selectVkPhysicalDevice(std::vector<VkPhysicalDevice>& devices);
		};
	}
}
