// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/vulkan/engine/wrappers/device.h>
#include <ao/vulkan/utilities/vulkan.h>
#include <ao/vulkan/engine/settings.h>
#include <vulkan/vulkan.hpp>

namespace ao::test {
	struct VkInstance {
	public:
		std::shared_ptr<vk::Instance> instance;
		std::shared_ptr<vulkan::Device> device;

		/// <summary>
		/// Method to init vulkan
		/// </summary>
		/// <returns>Successfull or not</returns>
		bool init();
	};

	/* IMPLEMENTATION */

	bool VkInstance::init() {
		vulkan::EngineSettings settings = vulkan::EngineSettings(vulkan::WindowSettings("TEST", 0, 0), vulkan::CoreSettings(std::thread::hardware_concurrency(), true));

		try {
			// Create instance
			this->instance = std::make_shared<vk::Instance>(vulkan::utilities::createVkInstance(settings, {}));

			// Get GPUs
			std::vector<vk::PhysicalDevice> devices = vulkan::utilities::vkPhysicalDevices(*this->instance);

			// Check count
			if (devices.empty()) {
				return false;
			}

			// Select a vk::PhysicalDevice & wrap it
			this->device = std::make_shared<vulkan::Device>(devices[0]);

			// Init logical device
			this->device->initLogicalDevice({}, {}, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		} catch (...) {
			return false;
		}
		return true;
	}
}
