#include "device.h"

ao::vulkan::Device::Device(vk::PhysicalDevice& device) {
	this->physical = device;

	// Get supported extensions
	this->extensions = ao::vulkan::utilities::vkExtensionProperties(this->physical);

	// Check count
	if (this->extensions.empty()) {
		LOGGER << LogLevel::WARN << "Extensions vector is empty";
	}
}

ao::vulkan::Device::~Device() {
	this->logical.destroyCommandPool(this->transferCommandPool);

	this->logical.destroy();
}

void ao::vulkan::Device::initLogicalDevice(std::vector<char const*> deviceExtensions, std::vector<vk::PhysicalDeviceFeatures> deviceFeatures, vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags, vk::QueueFlagBits defaultQueue, bool swapChain) {
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = this->physical.getQueueFamilyProperties();
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	float const DEFAULT_QUEUE_PRIORITY = 0.0f;
	std::array<vk::QueueFlagBits, 5> const allFlags = {
	    vk::QueueFlagBits::eGraphics, vk::QueueFlagBits::eCompute,
	    vk::QueueFlagBits::eTransfer, vk::QueueFlagBits::eSparseBinding,
	    vk::QueueFlagBits::eProtected
	};

	// Check count
	if (queueFamilyProperties.empty()) {
		throw ao::core::Exception("Empty queueFamilyProperties");
	}

	// Find queues
	std::map<uint32_t, vk::QueueFlagBits> indexes;
	boost::optional<uint32_t> defaultQueueIndex;
	for (auto& flag : allFlags) {
		if (qflags & flag) {
			uint32_t index = ao::vulkan::utilities::findQueueFamilyIndex(queueFamilyProperties, flag);

			// Check index
			if (index < 0) {
				throw ao::core::Exception("Fail to find a queueFamily that supports " + to_string(flag));
			}

			// Add index to set
			if (flag == defaultQueue) {
				defaultQueueIndex = index;
			}
			if (indexes.find(index) == indexes.end()) {
				indexes[index] = flag;
			}
		}
	}

	// Create info for queues
	for (auto& index : indexes) {
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), index.first, 1, &DEFAULT_QUEUE_PRIORITY));
	}

	// Request swap chain extension
	if (swapChain) {
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());
	deviceCreateInfo.setPEnabledFeatures(deviceFeatures.data());

	// Add extensions
	if (!deviceExtensions.empty()) {
		deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
		deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());
	}

	// Create device
	this->logical = this->physical.createDevice(deviceCreateInfo);

	// Build queue container
	if (!defaultQueueIndex) {
		throw core::Exception("Fail to find a queueFamily that supports graphics");
	}

	vk::Queue default = this->logical.getQueue(*defaultQueueIndex, 0);

	for (auto& flag : allFlags) {
		// Find in map
		auto it = std::find_if(indexes.begin(), indexes.end(), [flag](auto const& pair) -> bool {
			return pair.second == flag;
		});

		// Check iterator
		if (it != indexes.end()) {
			this->queues[flag] = QueueData(this->logical.getQueue(it->first, 0), it->first, queueFamilyProperties[it->first]);
		}
		else {
			this->queues[flag] = this->queues[defaultQueue];
		}
	}

	// Create command pool for transfert
	this->transferCommandPool = this->logical.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, this->queues[vk::QueueFlagBits::eTransfer].index));
}

uint32_t ao::vulkan::Device::memoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memoryProperties = this->physical.getMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	throw ao::core::Exception("Fail to find a matching memory type");
}
