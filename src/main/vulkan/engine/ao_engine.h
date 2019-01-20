// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>
#include <vector>
#include <mutex>
#include <tuple>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <ao/core/plugin.hpp>
#include <vulkan/vulkan.hpp>
#include <ctpl.h>

#include "containers/semaphore_container.h"
#include "../utilities/vulkan.h"
#include "wrappers/swapchain.h"
#include "wrappers/pipeline.h"
#include "wrappers/device.h"
#include "settings.h"

namespace ao::vulkan {
	using DrawInCommandBuffer = std::function<vk::CommandBuffer(int const, vk::CommandBufferInheritanceInfo const&, std::pair<std::array<vk::ClearValue, 2> const&, vk::Rect2D> const&)>;

	/// <summary>
	/// AOEngine class
	/// </summary>
	class AOEngine {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="settings">Settings</param>
		explicit AOEngine(EngineSettings const& settings);
		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~AOEngine();

		/// <summary>
		/// Methdo to run engine
		/// </summary>
		virtual void run();

		/// <summary>
		/// Method to add a plugin
		/// </summary>
		/// <param name="plugin">Plugin</param>
		void add(core::Plugin<AOEngine>* plugin);

		/// <summary>
		/// Method to set window's title
		/// </summary>
		/// <param name="title">Title</param>
		virtual void setWindowTitle(std::string const& title);

		/// <summary>
		/// Method to get settings
		/// </summary>
		/// <returns>Settings</returns>
		EngineSettings const& settings() const;
	protected:
		core::Logger LOGGER = core::Logger::getInstance<AOEngine>();

		std::vector<vk::Fence> waitingFences;
		SemaphoreContainer semaphores;

		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
		std::vector<vk::DescriptorPool>	descriptorPools;
		std::vector<vk::DescriptorSet> descriptorSets;

		std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView> stencilBuffer;
		std::vector<vk::Framebuffer> frameBuffers;
		u32 frameBufferIndex = 0;

		EngineSettings mSettings;

		std::shared_ptr<vk::Instance> instance;
		std::shared_ptr<SwapChain> swapchain;
		std::shared_ptr<Pipeline> pipeline;
		std::shared_ptr<Device> device;
		vk::RenderPass renderPass;

		/// <summary>
		/// Method to init vulkan
		/// </summary>
		void initVulkan();
		/// <summary>
		/// Method to prepare vulkan before running
		/// </summary>
		void prepareVulkan();
		/// <summary>
		/// Method to clean-up vulkan
		/// </summary>
		void freeVulkan();

		/// <summary>
		/// Method to set-up debugging
		/// </summary>
		void setUpDebugging();
		/// <summary>
		/// Method to create waiting fences
		/// </summary>
		void createWaitingFences();
		/// <summary>
		/// Method to create stencil buffer
		/// </summary>
		void createStencilBuffer();
		/// <summary>
		/// Method to create render pass
		/// </summary>
		void createRenderPass();
		/// <summary>
		/// Method to set-up frame buffers
		/// </summary>
		void setUpFrameBuffers();
		/// <summary>
		/// Method to re-create swap chain
		/// </summary>
		void recreateSwapChain();
		/// <summary>
		/// Method to create pipelines
		/// </summary>
		void createPipelines();
		/// <summary>
		/// Method to create semaphores
		/// </summary>
		void createSemaphores();

		/// <summary>
		/// Method to init surface
		/// </summary>
		/// <param name="surface">Surface reference</param>
		virtual void initSurface(vk::SurfaceKHR& surface) = 0;
		/// <summary>
		/// Method to init window
		/// </summary>
		virtual void initWindow() = 0;
		/// <summary>
		/// Method to clean-up window
		/// </summary>
		virtual void freeWindow() = 0;
		/// <summary>
		/// Method to know if window is iconified
		/// </summary>
		/// <returns></returns>
		virtual bool isIconified() const = 0;

		/// <summary>
		/// Method to define main loop
		/// </summary>
		virtual void loop();
		/// <summary>
		/// Method to define a loop condition
		/// </summary>
		/// <returns>True = continue to loop & False = stop looping</returns>
		virtual bool loopingCondition() const = 0;
		/// <summary>
		/// Method executed after frame was submitted
		/// </summary>
		virtual void afterFrameSubmitted();
		/// <summary>
		/// Method to wait until window is maximized
		/// </summary>
		virtual void waitMaximized() = 0;

		/// <summary>
		/// Method to render a frame
		/// </summary>
		void render();
		/// <summary>
		/// Method to prepare frame
		/// </summary>
		void prepareFrame();
		/// <summary>
		/// Method to submit frame
		/// </summary>
		void submitFrame();
		/// <summary>
		/// Method to update command buffers
		/// </summary>
		void updateCommandBuffers();

		/// <summary>
		/// Method to get vkInstance extensions
		/// </summary>
		/// <returns>Extensions</returns>
		virtual std::vector<char const*> instanceExtensions() const = 0;
		/// <summary>
		/// Method to get device extensions that must be enabled
		/// </summary>
		/// <returns>Extensions</returns>
		virtual std::vector<char const*> deviceExtensions() const;
		/// <summary>
		/// Method to get device features that must be enabled
		/// </summary>
		/// <returns>Features</returns>
		virtual std::vector<vk::PhysicalDeviceFeatures> deviceFeatures() const;
		/// <summary>
		/// Method to define queue flags
		/// </summary>
		/// <returns>Flags</returns>
		virtual vk::QueueFlags queueFlags() const;
		/// <summary>
		/// Method to define command pool flags
		/// </summary>
		/// <returns>Flags</returns>
		virtual vk::CommandPoolCreateFlags commandPoolFlags() const;
		/// <summary>
		/// Method to define debug report flags
		/// </summary>
		/// <returns>Flags</returns>
		virtual vk::DebugReportFlagsEXT debugReportFlags() const;

		/// <summary>
		/// Method to define a report callback
		/// </summary>
		/// <param name="flags">Flags</param>
		/// <param name="type">Type</param>
		/// <param name="srcObject">Source object</param>
		/// <param name="location">Location</param>
		/// <param name="msgCode">Message code</param>
		/// <param name="pLayerPrefix">Layer prefix</param>
		/// <param name="pMsg">Message</param>
		/// <param name="pUserData">Use data</param>
		/// <returns>True or False</returns>
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallBack(
			VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
			u64 srcObject, size_t location, s32 msgCode, const char* pLayerPrefix,
			const char* message, void* pUserData);

		/// <summary>
		/// Method to select a vk::PhysicalDevice
		/// </summary>
		/// <param name="devices">vk::PhysicalDevice</param>
		/// <returns>Index</returns>
		virtual size_t selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const;  // TODO: Optimize this, return reference

		/// <summary>
		/// Method to create pipeline layouts
		/// </summary>
		virtual void createPipelineLayouts() = 0;
		/// <summary>
		/// Method to set-up pipelines
		/// </summary>
		/// <returns></returns>
		virtual void setUpPipelines() = 0;
		/// <summary>
		/// Method to set-up render pass
		/// </summary>
		virtual void setUpRenderPass() = 0;
		/// <summary>
		/// Method to set-up vulkan buffers
		/// </summary>
		/// <returns></returns>
		virtual void setUpVulkanBuffers() = 0;
		/// <summary>
		/// Method to create secondary command buffers
		/// </summary>
		virtual void createSecondaryCommandBuffers() = 0;
		/// <summary>
		/// Method to get functions that will update secondary command buffers
		/// </summary>
		/// <returns>Function vector</returns>
		virtual std::vector<DrawInCommandBuffer> updateSecondaryCommandBuffers() = 0;
		/// <summary>
		/// Method to update uniform buffers
		/// </summary>
		virtual void updateUniformBuffers() = 0;
		/// <summary>
		/// Method to create descriptor set layouts
		/// </summary>
		virtual void createDescriptorSetLayouts() = 0;
		/// <summary>
		/// Method to create descriptor pools
		/// </summary>
		virtual void createDescriptorPools() = 0;
		/// <summary>
		/// Method to create descriptor sets
		/// </summary>
		virtual void createDescriptorSets() = 0;
	private:
		vk::DebugReportCallbackEXT debugCallBack;

		ctpl::thread_pool commandBufferPool;

		std::vector<core::Plugin<AOEngine>*> plugins;
		std::mutex pluginsMutex;

		/// <summary>
		/// Method to free plugins
		/// </summary>
		void freePlugins();
	};
}
