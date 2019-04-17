// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <vulkan/vulkan.hpp>

#include "allocator/allocator.h"

namespace ao::vulkan {
    /**
     * @brief Buffer
     *
     */
    class Buffer {
       public:
        /**
         * @brief Construct a new Buffer object
         *
         * @param allocator Allocator
         * @param size Buffer's size
         * @param usage Buffer usage
         */
        Buffer(std::shared_ptr<Allocator> allocator, vk::DeviceSize size, vk::BufferUsageFlags usage);

        /**
         * @brief Destroy the Buffer object
         *
         */
        virtual ~Buffer() {
            if (this->allocator_->own(*this->buffer_info)) {  // TODO: Necessary ???
                this->allocator_->free(*this->buffer_info);
            }
        }

        /**
         * @brief Get buffer info
         *
         * @return Allocator::BufferInfo Buffer info
         */
        virtual Allocator::BufferInfo const& info() const {
            return *this->buffer_info;
        }

        /**
         * @brief Get allocator
         *
         * @return std::shared_ptr<Allocator> const Allocator
         */
        virtual std::shared_ptr<Allocator> const allocator() const {
            return this->allocator_;
        }

       protected:
        std::shared_ptr<Allocator> allocator_;
        std::unique_ptr<Allocator::BufferInfo> buffer_info;
    };
}  // namespace ao::vulkan