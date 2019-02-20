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
        vk::Fence currentFence() const {
            return this->fences[this->frame_index];
        }

        /**
         * @brief Get current frame
         *
         * @return vk::Framebuffer Frame
         */
        vk::Framebuffer currentFrame() const {
            return this->frames[this->frame_index];
        }

        /**
         * @brief Get current command buffer
         *
         * @return vk::CommandBuffer& Command buffer
         */
        vk::CommandBuffer& currentCommand() {
            return this->commands[this->frame_index];
        }

        /**
         * @brief Get current frame index
         *
         * @return u32 Frame index
         */
        u32 frameIndex() const {
            return this->frame_index;
        }

        /**
         * @brief Get extent
         *
         * @return vk::Extent2D Extent
         */
        vk::Extent2D extent() const {
            return this->extent_;
        }

        /**
         * @brief Get color space
         *
         * @return vk::ColorSpaceKHR Color space
         */
        vk::ColorSpaceKHR colorSpace() const {
            return this->color_space;
        }

        /**
         * @brief Get color format
         *
         * @return vk::Format Format
         */
        vk::Format colorFormat() const {
            return this->color_format;
        }

        /**
         * @brief Get swapchain's size
         *
         * @return size_t Size
         */
        size_t size() const {
            return this->buffers.size();
        }

        /**
         * @brief Get state
         *
         * @return SwapchainState State
         */
        SwapchainState state() const {
            return this->state_;
        }

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
         * @return Swapchain* This
         */
        Swapchain* setSurface(vk::SurfaceKHR surface);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Swapchain>();

        std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
        std::vector<vk::Framebuffer> frames;
        std::vector<vk::Fence> fences;
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
