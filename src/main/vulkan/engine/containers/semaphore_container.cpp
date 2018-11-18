#include "semaphore_container.h"

ao::vulkan::SemaphoreContainer::~SemaphoreContainer() {
	if (!this->map.empty()) {
		ao::vulkan::SemaphoreContainer::clear();
	}
}

void ao::vulkan::SemaphoreContainer::clear() {
	std::vector<vk::Semaphore> cleared;

	// Destroy semaphores
	if (auto _device = ao::core::get(this->device)) {
		for (auto& pair : map) {
			for (auto& semaphore : pair.second.waits) {
				if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
					_device->logical.destroySemaphore(semaphore);
					cleared.push_back(semaphore);
				}
			}
			for (auto& semaphore : pair.second.signals) {
				if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
					_device->logical.destroySemaphore(semaphore);
					cleared.push_back(semaphore);
				}
			}
		}
	}
						   
	// Clear map
	core::MapContainer<std::string, SemaphorePair, std::map<std::string, SemaphorePair>::iterator>::clear();
	cleared.clear();
}
