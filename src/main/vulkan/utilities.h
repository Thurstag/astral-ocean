#pragma once

#include <vector>

#include "boost/throw_exception.hpp"
#include "engine_settings.h"
#include "vulkan/vulkan.h"

namespace ao {
	namespace vk {
		namespace utilities {
			/// <summary>
			/// Method to assert a VkResult
			/// </summary>
			/// <param name="result">VkResult</param>
			/// <param name="message">Exception's message on failure</param>
			inline void vkAssert(VkResult result, std::string message) {
				if (result != VkResult::VK_SUCCESS) {
					BOOST_THROW_EXCEPTION(std::exception(message.c_str()));
				}
			}

			/// <summary>
			/// Method to create a VkInstance
			/// </summary>
			/// <param name="settings">Engine settings</param>
			/// <param name="instance">VkInstance</param>
			/// <param name="extensions">Extensions</param>
			/// <returns>VkResult</returns>
			inline VkResult createVkInstance(EngineSettings& settings, VkInstance& instance, std::vector<char const*>& extensions) {
				std::vector<char const*> validationLayer{ "VK_LAYER_LUNARG_standard_validation" };

				// Create app info
				VkApplicationInfo appInfo = {};
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.apiVersion = VK_API_VERSION_1_0;

				// TODO: Optimize this part (retrieve info in settings) !!!
				appInfo.pApplicationName = "Hello Triangle";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = "No Engine";
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

				// Create instance info
				VkInstanceCreateInfo instanceInfo = {};
				instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				instanceInfo.pApplicationInfo = &appInfo;

				// Check validation
				if (settings.vkValidationLayers) {
					extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

					instanceInfo.enabledLayerCount = (uint32_t)validationLayer.size();
					instanceInfo.ppEnabledLayerNames = validationLayer.data();
				}
				instanceInfo.enabledExtensionCount = (uint32_t)extensions.size();
				instanceInfo.ppEnabledExtensionNames = extensions.data();

				// Create instance
				return vkCreateInstance(&instanceInfo, nullptr, &instance);
			}
		}
	}
}
