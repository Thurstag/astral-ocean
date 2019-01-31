// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <bitset>
#include <numeric>
#include <vector>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <ao/core/utilities/types.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>

#include "../engine/settings.h"

namespace ao::vulkan {
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
                    throw core::Exception(fmt::format("Unknown vk::Result: {0}", std::to_string(result)));
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
        inline vk::Instance createVkInstance(std::shared_ptr<EngineSettings> settings, std::vector<char const*> extensions) {
            std::vector<char const*> validationLayer{"VK_LAYER_LUNARG_standard_validation"};

            // Create app info
            vk::ApplicationInfo appInfo(
                settings->get<std::string>(ao::vulkan::settings::AppName, std::make_optional(std::string("Undefined"))).c_str(),
                settings->get<int>(ao::vulkan::settings::AppVersion, std::make_optional(VK_MAKE_VERSION(0, 0, 0))),
                settings->get<std::string>(ao::vulkan::settings::EngineName, std::make_optional(std::string("Astral-Ocean"))).c_str(),
                settings->get<int>(ao::vulkan::settings::EngineVersion, std::make_optional(VK_MAKE_VERSION(0, 0, 0))), VK_API_VERSION_1_1);

            // Add validation extension
            if (settings->get(ao::vulkan::settings::ValidationLayers, std::make_optional<bool>(false))) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            // Create instance info
            vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo)
                                                      .setEnabledExtensionCount(static_cast<u32>(extensions.size()))
                                                      .setPpEnabledExtensionNames(extensions.data());

            if (settings->get(ao::vulkan::settings::ValidationLayers, std::make_optional<bool>(false))) {
                instanceInfo.setEnabledLayerCount(static_cast<u32>(validationLayer.size())).setPpEnabledLayerNames(validationLayer.data());
            }

            // Create instance
            return vk::createInstance(instanceInfo);
        }

        /// <summary>
        /// Method to find a queue familly index that supports flag
        /// </summary>
        /// <param name="queueFamilyProperties">queueFamilyProperties</param>
        /// <param name="flag">Flag</param>
        /// <returns>Index</returns>
        inline u32 findQueueFamilyIndex(std::vector<vk::QueueFamilyProperties> const& queueFamilyProperties, vk::QueueFlagBits flag) {
            // clang-format off
            std::vector<VkQueueFlagBits> flags = {
                VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, 
				VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_PROTECTED_BIT
			};
            // clang-format on
            core::Logger LOGGER = core::Logger::GetInstance<Utilities>();

            // Calculate value of flags whitout flag parameter
            VkQueueFlags other = std::accumulate(flags.begin(), flags.end(), 0, [flag](VkQueueFlags result, VkQueueFlagBits f) {
                if (vk::QueueFlagBits(f) != flag) {
                    return result | f;
                }
                return result;
            });

            // Try to find a queue familly designed only for flag parameter
            for (u32 i = 0; i < queueFamilyProperties.size(); i++) {
                if ((queueFamilyProperties[i].queueFlags & flag) && ((VkQueueFlags(queueFamilyProperties[i].queueFlags) & other) == 0)) {
                    LOGGER << core::Logger::Level::debug << fmt::format("Found a queue that only supports: {0}", to_string(flag));
                    return i;
                }
            }

            // Try to find a queue familly that supports flag parameter
            for (u32 i = 0; i < queueFamilyProperties.size(); i++) {
                if (queueFamilyProperties[i].queueFlags & flag) {
                    return i;
                }
            }

            throw ao::core::Exception(fmt::format("Fail to find a queueFamily that supports: {0}", to_string(flag)));
        }

        /// <summary>
        /// Method to get supported depth format
        /// </summary>
        /// <param name="physicalDevice">vk::PhysicalDevice</param>
        /// <returns>vk::Format</returns>
        inline vk::Format getSupportedDepthFormat(vk::PhysicalDevice physicalDevice) {
            // clang-format off
            std::vector<vk::Format> formats = {
				vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint,
                vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm
			};
            // clang-format on

            for (auto& format : formats) {
                vk::FormatProperties formatProps = physicalDevice.getFormatProperties(format);

                // Check properties
                if (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
                    return format;
                }
            }
            throw core::Exception("Fail to find a suitable vk::Format");
        }

        /// <summary>
        /// Method to build a report of queue families
        /// </summary>
        /// <param name="queue_families">Queue families</param>
        /// <returns>Report</returns>
        inline std::string report(std::vector<vk::QueueFamilyProperties> const& queue_families) {
            std::stringstream ss;

            for (size_t i = 0; i < queue_families.size(); i++) {
                ss << fmt::format("{}: flags({}), capacity({})", i, vk::to_string(queue_families[i].queueFlags), queue_families[i].queueCount);

                if (i != queue_families.size() - 1) {
                    ss << std::endl;
                }
            }
            return ss.str();
        }
    }  // namespace utilities
}  // namespace ao::vulkan
