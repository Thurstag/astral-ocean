// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>

#include <ao/core/memory/map_container.hpp>
#include <ao/core/utilities/pointers.h>
#include <vulkan/vulkan.hpp>

#include "../wrappers/device.h"

namespace ao {
	namespace vulkan {
		struct SemaphorePair {
			std::vector<vk::Semaphore> signals;
			std::vector<vk::Semaphore> waits;

			/// <summary>
			/// Constructor
			/// </summary>
			SemaphorePair() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_waits">Waiting semaphores</param>
			/// <param name="_signals">Signal semaphores</param>
		 	explicit SemaphorePair(std::vector<vk::Semaphore> _waits , std::vector<vk::Semaphore> _signals) : waits(_waits), signals(_signals) {}
		};

		class SemaphoreContainer : public core::MapContainer<std::string, SemaphorePair, std::map<std::string, SemaphorePair>::iterator> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			SemaphoreContainer() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_device">Device</param>
			explicit SemaphoreContainer(std::weak_ptr<Device> _device) : device(_device) {}

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~SemaphoreContainer();

			virtual void clear() override;

		protected:
			std::weak_ptr<Device> device;
		};
	}
}

