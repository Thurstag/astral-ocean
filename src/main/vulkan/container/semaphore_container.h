// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>

#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    namespace structs {
        /**
         * @brief Semaphores container
         *
         */
        struct Semaphores {
            std::vector<vk::Semaphore> signals;
            std::vector<vk::Semaphore> waits;

            /**
             * @brief Construct a new Semaphores object
             *
             */
            Semaphores() = default;

            /**
             * @brief Construct a new Semaphores object
             *
             * @param waits Waiting semaphores
             * @param signals Signal semaphores
             */
            explicit Semaphores(std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) : waits(waits), signals(signals) {}
        };
    }  // namespace structs

    /**
     * @brief Semaphore container
     *
     */
    class SemaphoreContainer : public core::MapContainer<std::string, structs::Semaphores> {
       public:
        /**
         * @brief Construct a new SemaphoreContainer object
         *
         */
        SemaphoreContainer() = default;

        /**
         * @brief Construct a new SemaphoreContainer object
         *
         * @param device Device
         */
        explicit SemaphoreContainer(std::shared_ptr<vk::Device> device) : device(device) {}

        /**
         * @brief Destroy the SemaphoreContainer object
         *
         */
        virtual ~SemaphoreContainer();

        virtual void clear() override;

       protected:
        std::shared_ptr<vk::Device> device;
    };
}  // namespace ao::vulkan
