#include "semaphore_container.h"

ao::vulkan::SemaphoreContainer::~SemaphoreContainer() {
	if (!this->map.empty()) {
		ao::vulkan::SemaphoreContainer::clear();
	}
}

void ao::vulkan::SemaphoreContainer::clear() {
	std::vector<vk::Semaphore> cleared;

	// Destroy semaphores
	for (auto& pair : map) {
		for (auto& semaphore : pair.second.waits) {
			if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
				this->device->logical.destroySemaphore(semaphore);
				cleared.push_back(semaphore);
			}
		}
		for (auto& semaphore : pair.second.signals) {
			if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
				this->device->logical.destroySemaphore(semaphore);
				cleared.push_back(semaphore);
			}
		}
	}
						   
	// Clear map
	core::MapContainer<std::string, SemaphorePair>::clear();
	cleared.clear();
}
