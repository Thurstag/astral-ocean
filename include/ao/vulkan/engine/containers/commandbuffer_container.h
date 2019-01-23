// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

#include "../wrappers/device.h"

namespace ao::vulkan {
    namespace structs {
        struct CommandData {
            std::vector<vk::CommandBuffer> buffers;
            vk::CommandPool pool;

            /// <summary>
            /// Constructor
            /// </summary>
            CommandData() = default;

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="buffers">Buffers</param>
            /// <param name="pool">Pool</param>
            explicit CommandData(std::vector<vk::CommandBuffer> const& buffers, vk::CommandPool const& pool) : buffers(buffers), pool(pool) {}
        };
    }  // namespace structs

    /// <summary>
    /// CommandBufferContainer class
    /// </summary>
    class CommandBufferContainer : public core::MapContainer<std::string, structs::CommandData> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        explicit CommandBufferContainer(std::weak_ptr<Device> device);

        /// <summary>
        /// Destructor
        /// </summary>
        ~CommandBufferContainer();

        virtual void clear() override;

       protected:
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
