// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /**
     * @brief Fence statuses
     *
     */
    enum class FenceStatus { eSignaled, eUnsignaled, eDeviceLost, eDestroyed, eUnknown };

    /**
     * @brief vk::Fence wrapper
     *
     */
    class Fence {
       public:
        /**
         * @brief Construct a new Fence object
         *
         */
        Fence(){};

        /**
         * @brief Construct a new Fence object
         *
         * @param device Device
         */
        explicit Fence(std::shared_ptr<vk::Device> device);

        /**
         * @brief Destroy the Fence object
         *
         */
        virtual ~Fence() = default;

        /**
         * @brief Convert a vk::Result into a FenceStatus
         *
         * @param result Result
         * @return FenceStatus Status
         */
        static FenceStatus ToStatus(vk::Result result);

        /**
         * @brief Get status
         *
         * @return FenceStatus Status
         */
        FenceStatus status() const;

        /**
         * @brief Destroy fence
         *
         */
        void destroy();

        /**
         * @brief Reset fence
         *
         */
        void reset();

        /**
         * @brief Wait for fence to become signaled
         *
         * @param timeout Timeout
         */
        void wait(u64 timeout = (std::numeric_limits<u64>::max)()) const;

        /**
         * @brief Implicit conversion into vk::Fence
         *
         * @return vk::Fence fence
         */
        operator vk::Fence() const {
            return this->fence->first;
        }

        /**
         * @brief Implicit conversion into boolean
         *
         * @return true Fence exists
         * @return false Fence doesn't exist
         */
        operator bool() const {
            return this->fence && this->fence->second != FenceStatus::eDestroyed;
        }

       protected:
        std::shared_ptr<std::pair<vk::Fence, FenceStatus>> fence;
        std::shared_ptr<vk::Device> device;

        /**
         * @brief Assert fence existence
         *
         */
        void(assert)() const;
    };
}  // namespace ao::vulkan