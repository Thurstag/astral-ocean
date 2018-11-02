#pragma once

#include <numeric>
#include <bitset>
#include <vector>

#include <ao/core/exception.h>
#include <ao/core/logger.h>
#include <vulkan/vulkan.hpp>

#include "../engine/settings.h"

namespace ao {
	namespace vulkan {
		struct Utilities {};
		namespace utilities {
			/// <summary>
			/// Method to convert a	VkResult into a vk::Result
			/// </summary>
			inline vk::Result to_result(VkResult result) {
				switch (result) {
					case VK_SUCCESS:
						return vk::Result::eSuccess;
					case VK_NOT_READY:
						return vk::Result::eNotReady;
					case VK_TIMEOUT:
						return vk::Result::eTimeout;
					case VK_EVENT_SET:
						return vk::Result::eEventSet;
					case VK_EVENT_RESET:
						return vk::Result::eEventReset;
					case VK_INCOMPLETE:
						return vk::Result::eIncomplete;
					case VK_ERROR_OUT_OF_HOST_MEMORY:
						return vk::Result::eErrorOutOfHostMemory;
					case VK_ERROR_OUT_OF_DEVICE_MEMORY:
						return vk::Result::eErrorOutOfDeviceMemory;
					case VK_ERROR_INITIALIZATION_FAILED:
						return vk::Result::eErrorInitializationFailed;
					case VK_ERROR_DEVICE_LOST:
						return vk::Result::eErrorDeviceLost;
					case VK_ERROR_MEMORY_MAP_FAILED:
						return vk::Result::eErrorMemoryMapFailed;
					case VK_ERROR_LAYER_NOT_PRESENT:
						return vk::Result::eErrorLayerNotPresent;
					case VK_ERROR_EXTENSION_NOT_PRESENT:
						return vk::Result::eErrorExtensionNotPresent;
					case VK_ERROR_FEATURE_NOT_PRESENT:
						return vk::Result::eErrorFeatureNotPresent;
					case VK_ERROR_INCOMPATIBLE_DRIVER:
						return vk::Result::eErrorIncompatibleDriver;
					case VK_ERROR_TOO_MANY_OBJECTS:
						return vk::Result::eErrorTooManyObjects;
					case VK_ERROR_FORMAT_NOT_SUPPORTED:
						return vk::Result::eErrorFormatNotSupported;
					case VK_ERROR_FRAGMENTED_POOL:
						return vk::Result::eErrorFragmentedPool;
					case VK_ERROR_OUT_OF_POOL_MEMORY:
						return vk::Result::eErrorOutOfPoolMemory;
					case VK_ERROR_INVALID_EXTERNAL_HANDLE:
						return vk::Result::eErrorInvalidExternalHandle;
					case VK_ERROR_SURFACE_LOST_KHR:
						return vk::Result::eErrorSurfaceLostKHR;
					case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
						return vk::Result::eErrorNativeWindowInUseKHR;
					case VK_SUBOPTIMAL_KHR:
						return vk::Result::eSuboptimalKHR;
					case VK_ERROR_OUT_OF_DATE_KHR:
						return vk::Result::eErrorOutOfDateKHR;
					case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
						return vk::Result::eErrorIncompatibleDisplayKHR;
					case VK_ERROR_VALIDATION_FAILED_EXT:
						return vk::Result::eErrorValidationFailedEXT;
					case VK_ERROR_INVALID_SHADER_NV:
						return vk::Result::eErrorInvalidShaderNV;
					case VK_ERROR_FRAGMENTATION_EXT:
						return vk::Result::eErrorFragmentationEXT;
					case VK_ERROR_NOT_PERMITTED_EXT:
						return vk::Result::eErrorNotPermittedEXT;
					default:
						throw core::Exception("Unknown vk::Result: " + std::to_string(result));
				}
			}

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
				vk::createResultValue(to_result(result), message.c_str());
			}

			/// <summary>
			/// Method to assert a vk::Result
			/// </summary>
			/// <param name="result">vk::Result</param>
			/// <param name="message">Exception's message on failure</param>
			inline void vkAssert(vk::Result result, std::string message) {
				vk::createResultValue(result, message.c_str());
			}

			/// <summary>
			/// Method to create a vk::Instance
			/// </summary>
			/// <param name="settings">Engine settings</param>
			/// <param name="extensions">Extensions</param>
			/// <returns>vk::Instance</returns>
			inline vk::Instance createVkInstance(EngineSettings& settings, std::vector<char const*>& extensions) {
				std::vector<char const*> validationLayer{ "VK_LAYER_LUNARG_standard_validation" };

				// Create app info (TODO: Optimize this part (retrieve info in settings) !!!)
				vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

				// Create instance info
				vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &appInfo);
				instanceInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
				instanceInfo.setPpEnabledExtensionNames(extensions.data());

				// Create instance
				return vk::createInstance(instanceInfo);
			}

			/// <summary>
			/// Method to get all vk::PhysicalDevices
			/// </summary>
			/// <param name="instance">vk::Instance</param>
			/// <returns>GPUs</returns>
			inline std::vector<vk::PhysicalDevice> vkPhysicalDevices(vk::Instance& instance) {
				std::string error = "Fail to enumerate vk::PhysicalDevices";
				std::vector<vk::PhysicalDevice> devices;
				uint32_t count;

				// Get count
				vkAssert(instance.enumeratePhysicalDevices(&count, nullptr), error);

				// Adapt vector
				devices.resize(count);

                // Get vk::PhysicalDevices
				vkAssert(instance.enumeratePhysicalDevices(&count, devices.data()), error);
				return devices;
			}

			/// <summary>
			/// Method to get vk::ExtensionProperties
			/// </summary>
			/// <param name="device">vk::PhysicalDevice</param>
			/// <returns>vk::ExtensionProperties</returns>
			inline std::vector<vk::ExtensionProperties> vkExtensionProperties(vk::PhysicalDevice& device) {
				std::vector<vk::ExtensionProperties> extensions;
				uint32_t count;

				// Get count
				device.enumerateDeviceExtensionProperties(nullptr, &count, nullptr);

				// Adapt vector
				extensions.resize(count);

				// Get vk::ExtensionProperties
				device.enumerateDeviceExtensionProperties(nullptr, &count, extensions.data());
				return extensions;
			}

			/// <summary>
			/// Method to find a queue familly index that supports flag
			/// </summary>
			/// <param name="queueFamilyProperties">queueFamilyProperties</param>
			/// <param name="flag">Flag</param>
			inline uint32_t findQueueFamilyIndex(std::vector<vk::QueueFamilyProperties>& queueFamilyProperties, vk::QueueFlagBits flag) {
				std::vector<VkQueueFlagBits> flags = { VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT };

				// Calculate value of flags whitout flag parameter
				VkQueueFlags other = std::accumulate(flags.begin(), flags.end(), 0, [flag](VkQueueFlags result, VkQueueFlagBits f) { 
					if (vk::QueueFlagBits(f) != flag) {
						return result | f;
					}
					return result;
				});

				// Try to find a queue familly designed only for flag parameter
				for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
					if ((queueFamilyProperties[i].queueFlags & flag) && ((VkQueueFlags(queueFamilyProperties[i].queueFlags) & other) == 0)) {
						core::Logger::getInstance<Utilities>() << LogLevel::DEBUG << "Found a queueFamily that only support: " << std::bitset<8>(static_cast<int>(flag));
						return i;
					}
				}

				// Try to find a queue familly that supports flag parameter
				for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
					if (queueFamilyProperties[i].queueFlags & flag) {
						return i;
					}
				}
				return -1;
			}

			/// <summary>
			/// Method to get supported depth format
			/// </summary>
			/// <param name="physicalDevice">vk::PhysicalDevice</param>
			/// <returns>vk::Format</returns>
			inline vk::Format getSupportedDepthFormat(vk::PhysicalDevice& physicalDevice) {
				std::vector<vk::Format> formats = {
					vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat,
					vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint,
					vk::Format::eD16Unorm
				};

				for (vk::Format& format : formats) {
					vk::FormatProperties formatProps = physicalDevice.getFormatProperties(format);

					// Check properties
					if (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
						return format;
					}
				}
				throw core::Exception("Fail to find a suitable vk::Format");
			}

			/// <summary>
			/// Method to get surface formats
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="surface">Surface</param>
			/// <returns>Surface formats</returns>
			inline std::vector<vk::SurfaceFormatKHR> surfaceFormatKHRs(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
				std::string error = "Fail to get supported surface formats";
				std::vector<vk::SurfaceFormatKHR> formats;
				uint32_t count;

				// Get count
				vkAssert(device.getSurfaceFormatsKHR(surface, &count, nullptr), error);

				// Adapt vector
				formats.resize(count);

				// Get vk::SurfaceFormatKHRs
				vkAssert(device.getSurfaceFormatsKHR(surface, &count, formats.data()), error);
				return formats;
			}

			/// <summary>
			/// Method to get present modes
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="surface">Surface</param>
			/// <returns>Present modes</returns>
			inline std::vector<vk::PresentModeKHR> presentModeKHRs(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
				std::string error = "Fail to get present modes";
				std::vector<vk::PresentModeKHR> modes;
				uint32_t count;

				// Get count
				vkAssert(device.getSurfacePresentModesKHR(surface, &count, nullptr), error);

				// Adapt vector
				modes.resize(count);

				// Get vk::PresentModeKHR
				vkAssert(device.getSurfacePresentModesKHR(surface, &count, modes.data()), error);
				return modes;
			}

			/// <summary>
			/// Method to get swap chain images
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="swapChain">Swap chain</param>
			/// <returns>Images</returns>
			inline std::vector<vk::Image> swapChainImages(vk::Device& device, vk::SwapchainKHR& swapChain) {
				std::string error = "Fail to get swap chain images";
				std::vector<vk::Image> images;
				uint32_t count;

				// Get count
				vkAssert(device.getSwapchainImagesKHR(swapChain, &count, nullptr), error);

				// Adapt vector
				images.resize(count);

				// Get vk::Images
				vkAssert(device.getSwapchainImagesKHR(swapChain, &count, images.data()), error);
				return images;
			}

			/// <summary>
			/// Method to convert a vk::PresentModeKHR into a string
			/// </summary>
			/// <param name="presentMode">vk::PresentModeKHR</param>
			/// <returns>vk::PresentModeKHR string representation</returns>
			inline std::string to_string(vk::PresentModeKHR presentMode) {
				switch (presentMode) {
					case vk::PresentModeKHR::eImmediate:
						return "PresentModeKHR::eImmediate";
					case vk::PresentModeKHR::eMailbox:
						return "PresentModeKHR::eMailbox";
					case vk::PresentModeKHR::eFifo:
						return "PresentModeKHR::eFifo";
					case vk::PresentModeKHR::eFifoRelaxed:
						return "PresentModeKHR::eFifoRelaxed";
					case vk::PresentModeKHR::eSharedDemandRefresh:
						return "PresentModeKHR::eSharedDemandRefresh";
					case vk::PresentModeKHR::eSharedContinuousRefresh:
						return "PresentModeKHR::eSharedContinuousRefresh";
					default:
						throw core::Exception("Unknown vk::PresentModeKHR: " + std::to_string(static_cast<int>(presentMode)));
				}
			}
		}
	}
}
