#pragma once

#include <mutex>
#include <tuple>

#include <ao/core/exception.h>
#include <ao/core/plugin.h>
#include <ao/core/logger.h>
#include <vulkan/vulkan.hpp>

#include "engine_settings.h"
#include "ao_swapchain.h"
#include "vk_utilities.h"
#include "ao_device.h"

namespace ao {
	namespace vulkan {
		/// <summary>
		/// AOEngine class
		/// </summary>
		class AOEngine {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="settings">Settings</param>
			AOEngine(EngineSettings settings);
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
			/// <param name="title"></param>
			virtual void setWindowTitle(std::string title);

			/// <summary>
			/// Method to get settings
			/// </summary>
			/// <returns>Settings</returns>
			EngineSettings settings();
		protected:
			core::Logger LOGGER = core::Logger::getInstance<AOEngine>();
			
			std::pair<vk::Semaphore, vk::Semaphore> semaphores; // First = Present semaphore & Second = Render semaphore
			std::vector<vk::Fence> waitingFences;
			vk::Queue queue;

			std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView> stencilBuffer;
			std::vector<vk::Framebuffer> frameBuffers;
			uint32_t frameBufferIndex = 0;

			vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlags(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			vk::SubmitInfo submitInfo;

			EngineSettings _settings;

			vk::PipelineCache pipelineCache;
			vk::RenderPass renderPass;
			AOSwapChain* swapchain;
			vk::Instance instance;
			AODevice* device;

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
			/// Method to create waiting fences
			/// </summary>
			void createWaitingFences();
			/// <summary>
			/// Method to create stencil buffer
			/// </summary>
			void createStencilBuffer();
			/// <summary>
			/// Method to set-up render pass
			/// </summary>
			void setUpRenderPass();
			/// <summary>
			/// Method to set-up frame buffers
			/// </summary>
			void setUpFrameBuffers();
			/// <summary>
			/// Method to re-create swap chain
			/// </summary>
			void recreateSwapChain();

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
			virtual bool isIconified() = 0;

			/// <summary>
			/// Method to define main loop
			/// </summary>
			virtual void loop();
			/// <summary>
			/// Method to define a loop condition
			/// </summary>
			/// <returns>True = continue to loop & False = stop looping</returns>
			virtual bool loopingCondition() = 0;
			/// <summary>
			/// Method executed on each loop iteration
			/// </summary>
			virtual void onLoopIteration();
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
			/// Method to get vkInstance extensions
			/// </summary>
			/// <returns>Extensions</returns>
			virtual std::vector<char const*> instanceExtensions() = 0;
			/// <summary>
			/// Method to get device extensions that must be enabled on device
			/// </summary>
			/// <returns></returns>
			virtual std::vector<char const*> deviceExtensions();
			/// <summary>
			/// Method to define queue flags
			/// </summary>
			/// <returns>flags</returns>
			virtual vk::QueueFlags queueFlags();
			/// <summary>
			/// Method to define command pool flags
			/// </summary>
			/// <returns>flags</returns>
			virtual vk::CommandPoolCreateFlags commandPoolFlags();

			/// <summary>
			/// Method to select a vk::PhysicalDevice
			/// </summary>
			/// <param name="devices">vk::PhysicalDevice</param>
			/// <returns>Index</returns>
			virtual size_t selectVkPhysicalDevice(std::vector<vk::PhysicalDevice>& devices);  // TODO: Optimize this
		private:
			std::vector<core::Plugin<AOEngine>*> plugins;
			std::mutex pluginsMutex;

			/// <summary>
			/// Method to free plugins
			/// </summary>
			void freePlugins();
		};
	}
}
