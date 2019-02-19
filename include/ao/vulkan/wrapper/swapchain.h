// Copyright 2018-2019 Astral-Ocean Project
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
    /**
     * @brief Swapchain states
     *
     */
    enum class SwapchainState { eIdle, eAcquireImage, eReset };

    /**
     * @brief vk::Swapchain wrapper
     *
     */
    class Swapchain {
       public:
        using StencilBuffer = std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView>;

        /**
         * @brief Construct a new Swapchain object
         *
         * @param instance Instance
         * @param device Device
         */
        Swapchain(std::weak_ptr<vk::Instance> instance, std::weak_ptr<Device> device);

        /**
         * @brief Destroy the Swapchain object
         *
         */
        virtual ~Swapchain();

        /**
         * @brief Initialize swapchain
         *
         * @param win_width Window's width
         * @param win_height Window's height
         * @param vsync Enable vsync
         * @param stencil_buffer Enable stencil buffer
         */
        void init(u64& win_width, u64& win_height, bool vsync, bool stencil_buffer);

        /**
         * @brief Initialize surface
         *
         */
        void initSurface();

        /**
         * @brief Create framebuffers
         *
         * @param render_pass Render pass
         */
        void createFramebuffers(vk::RenderPass render_pass);

        /**
         * @brief Destroy framebuffers
         *
         */
        void destroyFramebuffers();

        /**
         * @brief Create a stencil buffer
         *
         */
        void createStencilBuffer();

        /**
         * @brief Detsroy a stencil buffer
         *
         */
        void destroyStencilBuffer();

        /**
         * @brief Get current fence
         *
         * @return vk::Fence Fence
         */
        vk::Fence currentFence();

        /**
         * @brief Get current frame
         *
         * @return vk::Framebuffer Frame
         */
        vk::Framebuffer currentFrame();

        /**
         * @brief Get current command buffer
         *
         * @return vk::CommandBuffer& Command buffer
         */
        vk::CommandBuffer& currentCommand();

        /**
         * @brief Get current frame index
         *
         * @return u32 Frame index
         */
        u32 currentFrameIndex();

        /**
         * @brief Get extent
         *
         * @return vk::Extent2D Extent
         */
        vk::Extent2D extent();

        /**
         * @brief Get color space
         *
         * @return vk::ColorSpaceKHR Color space
         */
        vk::ColorSpaceKHR colorSpace();

        /**
         * @brief Get color format
         *
         * @return vk::Format Format
         */
        vk::Format colorFormat();

        /**
         * @brief Get swapchain's size
         *
         * @return size_t Size
         */
        size_t size();

        /**
         * @brief Get state
         *
         * @return SwapchainState State
         */
        SwapchainState state();

        /**
         * @brief Acquire next image
         *
         * @param acquire Acquire semaphore
         * @return vk::Result Result
         */
        vk::Result acquireNextImage(vk::Semaphore acquire);

        /**
         * @brief Enqueue an image
         *
         * @param waiting_semaphores Waiting semaphores
         * @return vk::Result Result
         */
        vk::Result enqueueImage(vk::ArrayProxy<vk::Semaphore> waiting_semaphores);

        /**
         * @brief Set surface
         *
         * @param surface Surface
         * @return Swapchain* Swapchain
         */
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
        SwapchainState state_;
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
