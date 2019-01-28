// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <array>
#include <limits>
#include <tuple>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "../containers/commandbuffer_container.h"
#include "device.h"

namespace ao::vulkan {
    /// <summary>
    /// Wrapper for vulkan swapchain
    /// </summary>
    struct Swapchain {
       public:
        using StencilBuffer = std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView>;

        std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
        std::vector<vk::Fence> waiting_fences;
        std::vector<vk::Framebuffer> frames;
        vk::SwapchainKHR swapchain;
        vk::Queue present_queue;
        vk::SurfaceKHR surface;
        u32 frame_index;

        std::optional<StencilBuffer> stencil_buffer;
        CommandBufferContainer commands;
        vk::CommandPool command_pool;

        vk::ColorSpaceKHR color_space;
        vk::Extent2D current_extent;
        vk::Format color_format;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="instance">Instance</param>
        /// <param name="device">Device</param>
        Swapchain(std::weak_ptr<vk::Instance> instance, std::weak_ptr<Device> device);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Swapchain();

        /// <summary>
        /// Method to init swap chain
        /// </summary>
        /// <param name="win_width">Window's width</param>
        /// <param name="win_height">Window's height</param>
        /// <param name="vsync">Vsync enabled</param>
        /// <param name="stencil_buffer">Stencil buffer enabled</param>
        void init(u64& win_width, u64& win_height, bool vsync, bool stencil_buffer);

        /// <summary>
        /// Method to init surface
        /// </summary>
        void initSurface();

        /// <summary>
        /// Method to prepare swapchain
        /// </summary>
        void prepare();

        /// <summary>
        /// Method to create command buffers
        /// </summary>
        void createCommandBuffers();

        /// <summary>
        /// Method to create framebuffers
        /// </summary>
        /// <param name="render_pass">Render pass</param>
        void createFramebuffers(vk::RenderPass render_pass);

        /// <summary>
        /// Method to destroy framebuffers
        /// </summary>
        void destroyFramebuffers();

        /// <summary>
        /// Method to free command buffers
        /// </summary>
        void freeCommandBuffers();

        /// <summary>
        /// Method to create stencil buffer
        /// </summary>
        void createStencilBuffer();

        /// <summary>
        /// Method to destroy stencil buffer
        /// </summary>
        void destroyStencilBuffer();

        /// <summary>
        /// Method to get current fence
        /// </summary>
        /// <returns>Current fence</returns>
        vk::Fence& currentFence();

        /// <summary>
        /// Method to get current frame
        /// </summary>
        /// <returns>Current Frame</returns>
        vk::Framebuffer& currentFrame();

        /// <summary>
        /// Method to get next image
        /// </summary>
        /// <param name="acquire">Acquire semaphore</param>
        vk::Result nextImage(vk::Semaphore acquire);

        /// <summary>
        /// Method to enqueue an image
        /// </summary>
        /// <param name="waiting_semaphores">Waiting semaphores</param>
        /// <returns>Result</returns>
        vk::Result enqueueImage(std::vector<vk::Semaphore>& waiting_semaphores);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Swapchain>();

        std::weak_ptr<vk::Instance> instance;
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
