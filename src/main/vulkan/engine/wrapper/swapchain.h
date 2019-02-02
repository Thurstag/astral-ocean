// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <array>
#include <limits>
#include <tuple>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "command_pool.h"
#include "device.h"

namespace ao::vulkan {
    /// <summary>
    /// Wrapper for vulkan swapchain
    /// </summary>
    class Swapchain {
       public:
        using StencilBuffer = std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView>;

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
        /// Method to create framebuffers
        /// </summary>
        /// <param name="render_pass">Render pass</param>
        void createFramebuffers(vk::RenderPass render_pass);

        /// <summary>
        /// Method to destroy framebuffers
        /// </summary>
        void destroyFramebuffers();

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
        vk::Fence currentFence();

        /// <summary>
        /// Method to get current frame
        /// </summary>
        /// <returns>Current Frame</returns>
        vk::Framebuffer currentFrame();

        /// <summary>
        /// Method to get current command buffer
        /// </summary>
        /// <returns>Command buffer</returns>
        vk::CommandBuffer& currentCommand();

        /// <summary>
        /// Method to get current frame index
        /// </summary>
        /// <returns>Frame index</returns>
        u32 currentFrameIndex();

        /// <summary>
        /// Method to get extent
        /// </summary>
        /// <returns>Extent</returns>
        vk::Extent2D extent();

        /// <summary>
        /// Method to get color space
        /// </summary>
        /// <returns>Color space</returns>
        vk::ColorSpaceKHR colorSpace();

        /// <summary>
        /// Method to get color format
        /// </summary>
        /// <returns>Color format</returns>
        vk::Format colorFormat();

        /// <summary>
        /// Method to get swapchain's size
        /// </summary>
        /// <returns>Image count</returns>
        size_t size();

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

        /// <summary>
        /// Method to set surface
        /// </summary>
        /// <param name="surface">Surface</param>
        /// <returns>This</returns>
        Swapchain* setSurface(vk::SurfaceKHR surface);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Swapchain>();

        std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
        std::vector<vk::Fence> waiting_fences;
        std::vector<vk::Framebuffer> frames;
        vk::SwapchainKHR swapchain;
        u32 surface_images_count;
        vk::Queue present_queue;
        vk::SurfaceKHR surface;
        u32 frame_index;

        std::optional<StencilBuffer> stencil_buffer;

        vk::ColorSpaceKHR color_space;
        vk::Format color_format;
        vk::Extent2D extent_;

        std::unique_ptr<CommandPool> command_pool;
        std::vector<vk::CommandBuffer> commands;
        std::weak_ptr<vk::Instance> instance;
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
