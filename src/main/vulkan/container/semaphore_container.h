// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>

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
     * TODO: Disable copy
     *
     */
    class SemaphoreContainer {
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

        /**
         * @brief Operator []
         *
         * @param index Index Index
         * @return structs::Semaphores& Element
         */
        virtual structs::Semaphores& operator[](size_t index) {
            return this->content[index];
        }

        /**
         * @brief Get element at index {index}
         *
         * @param index Index
         * @return structs::Semaphores& Element
         */
        virtual structs::Semaphores& at(size_t index) {
            return this->content[index];
        }

        /**
         * @brief Clear content
         *
         */
        virtual void clear();

        /**
         * @brief Resize container
         *
         * @param size Size
         */
        virtual void resize(size_t size);

       protected:
        std::shared_ptr<vk::Device> device;

        std::vector<structs::Semaphores> content;
    };
}  // namespace ao::vulkan
