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

#include "settings.h"

namespace ao::vulkan {
    struct Utilities {};

    namespace utilities {
        /**
         * @brief Check result
         *
         * @param result Result
         * @return true Result == VK_SUCCESS
         * @return false Result != VK_SUCCESS
         */
        inline bool vkCheck(VkResult result) {
            return result == VkResult::VK_SUCCESS;
        }

        /**
         * @brief Assert a result
         *
         * @param result Result
         * @param message Message on failure
         */
        inline void vkAssert(VkResult result, std::string message) {
            vk::createResultValue(vk::Result(result), message.c_str());
        }

        /**
         * @brief Assert a result
         *
         * @param result Result
         * @param message Message on failure
         */
        inline void vkAssert(vk::Result result, std::string message) {
            vk::createResultValue(result, message.c_str());
        }

        /**
         * @brief Create a Vk::Instance object
         *
         * @param settings Settings
         * @param extensions Extenions to enable
         * @return vk::Instance Instance
         */
        inline vk::Instance createVkInstance(std::shared_ptr<EngineSettings> settings, std::vector<char const*> extensions) {
            std::vector<char const*> validationLayer{"VK_LAYER_LUNARG_standard_validation"};

            // Create app info
            vk::ApplicationInfo appInfo(
                settings->get<std::string>(ao::vulkan::settings::AppName, std::make_optional<std::string>("Undefined")).c_str(),
                settings->get<int>(ao::vulkan::settings::AppVersion, std::make_optional(VK_MAKE_VERSION(0, 0, 0))),
                settings->get<std::string>(ao::vulkan::settings::EngineName, std::make_optional<std::string>("Astral-Ocean")).c_str(),
                settings->get<int>(ao::vulkan::settings::EngineVersion, std::make_optional(VK_MAKE_VERSION(0, 0, 0))), VK_API_VERSION_1_1);

            // Add validation extension
            if (settings->get(ao::vulkan::settings::ValidationLayers, std::make_optional(false)) &&
                std::find(extensions.begin(), extensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensions.end()) {
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

        /**
         * @brief Find a family queue that supports {flag}
         *
         * @param queueFamilyProperties Queue families
         * @param flag Flags
         * @return u32 Family queue index
         */
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
                    LOGGER << core::Logger::Level::trace << fmt::format("Found a queue that only supports: {0}", to_string(flag));
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

        /**
         * @brief Get the supported depth format for a device
         *
         * @param physicalDevice Device
         * @return vk::Format Format
         */
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

        /**
         * @brief Build to report of queue families
         *
         * @param queue_families Queue families
         * @return std::string Report
         */
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
