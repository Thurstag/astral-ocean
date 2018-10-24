#pragma once

#include <vector>

#include "boost/throw_exception.hpp"
#include "engine_settings.h"
#include "vulkan/vulkan.h"

namespace ao {
	namespace vk {
		namespace utilities {

			/// <summary>
			/// Method to check a VkResult 
			/// </summary>
			/// <param name="result">VkResult</param>
			/// <returns>VkResult is VK_SUCCESS</returns>
			inline bool vkCheck(VkResult result) {
				return result == VkResult::VK_SUCCESS;
			}

			/// <summary>
			/// Method to assert a VkResult
			/// </summary>
			/// <param name="result">VkResult</param>
			/// <param name="message">Exception's message on failure</param>
			inline void vkAssert(VkResult result, std::string message) {
				if (!vkCheck(result)) {
					BOOST_THROW_EXCEPTION(std::exception((message + ". Reason: " + std::to_string(result)).c_str()));
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

			/// <summary>
			/// Method to init debugging
			/// </summary>
			/// <param name="instance">VkInstance</param>
			/// <param name="flags">Report flags</param>
			/// <param name="callBack">Callback</param>
			inline void initDebugging(VkInstance& instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callback) {
				// TODO
			}

			/// <summary>
			/// Method to get all VkPhysicalDevices
			/// </summary>
			/// <param name="instance">VkInstance</param>
			/// <returns>GPUs</returns>
			inline std::vector<VkPhysicalDevice> vkPhysicalDevices(VkInstance& instance) {
				std::string error = "Fail to enumerate VkPhysicalDevices";
				std::vector<VkPhysicalDevice> devices;
				uint32_t count;

				// Get count
				ao::vk::utilities::vkAssert(vkEnumeratePhysicalDevices(instance, &count, nullptr), error);

				// Adapt vector
				devices.resize(count);

                // Get VkPhysicalDevices
				ao::vk::utilities::vkAssert(vkEnumeratePhysicalDevices(instance, &count, devices.data()), error);
				return devices;
			}

			/// <summary>
			/// Method to get VkQueueFamilyProperties 
			/// </summary>
			/// <param name="device">VkInstance</param>
			/// <returns>VkQueueFamilyProperties</returns>
			inline std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties(VkPhysicalDevice& device) {
				std::vector<VkQueueFamilyProperties> properties;
				uint32_t count;

				// Get count
				vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

				// Adapt vector
				properties.resize(count);

				// Get VkQueueFamilyProperties
				vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());
				return properties;
			}

			/// <summary>
			/// Method to get VkExtensionProperties
			/// </summary>
			/// <param name="device">VkInstance</param>
			/// <returns>VkExtensionProperties</returns>
			inline std::vector<VkExtensionProperties> vkExtensionProperties(VkPhysicalDevice& device) {
				std::vector<VkExtensionProperties> extensions;
				uint32_t count;

				// Get count
				vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);

				// Adapt vector
				extensions.resize(count);

				// Get VkQueueFamilyProperties
				vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
				return extensions;
			}
		}
	}
}
