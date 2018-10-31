#pragma once

#include <string>

#include <ao/core/exception.h>
#include <vulkan/vulkan.hpp>

namespace ao {
	namespace vulkan {
		namespace enums {
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
						throw ao::core::Exception("Unknown vk::Result: " + std::to_string(result));
				}
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
						return "Unknown vk::PresentModeKHR: " + std::to_string(static_cast<int>(presentMode));
				}
			}
		}
	}
}
