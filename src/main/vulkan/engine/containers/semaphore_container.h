// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>

#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

#include "../wrappers/device.h"

namespace ao::vulkan {
    namespace structs {
        struct Semaphores {
            std::vector<vk::Semaphore> signals;
            std::vector<vk::Semaphore> waits;

            /// <summary>
            /// Constructor
            /// </summary>
            Semaphores() = default;

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="waits">Waiting semaphores</param>
            /// <param name="signals">Signal semaphores</param>
            explicit Semaphores(std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) : waits(waits), signals(signals) {}
        };
    }  // namespace structs

    /// <summary>
    /// SemaphoreContainer class
    /// </summary>
    class SemaphoreContainer : public core::MapContainer<std::string, structs::Semaphores> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        SemaphoreContainer() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        explicit SemaphoreContainer(std::weak_ptr<Device> device) : device(device) {}

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~SemaphoreContainer();

        virtual void clear() override;

       protected:
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
