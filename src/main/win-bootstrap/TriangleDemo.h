#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <vector>

#include <ao/vulkan/engine/wrappers/buffers/device_buffer.hpp>
#include <ao/vulkan/engine/wrappers/shadermodule.h>
#include <ao/vulkan/engine/glfw_engine.h>
#include <ao/vulkan/engine/settings.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "vertex.hpp"

class TriangleDemo : public virtual ao::vulkan::GLFWEngine {
public:
	std::chrono::time_point<std::chrono::system_clock> clock;
	bool clockInit = false;

	std::vector<Vertex> vertices;
	std::vector<u16> indices;

	ao::vulkan::Buffer<Vertex*>* vertexBuffer;
	ao::vulkan::Buffer<u16*>* indexBuffer;

	TriangleDemo(ao::vulkan::EngineSettings settings) : ao::vulkan::GLFWEngine(settings), ao::vulkan::AOEngine(settings) {
		this->vertices = {
			{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
			{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{ {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} }
		};
		this->indices = { 0, 1, 2, 0 };
	};
	virtual ~TriangleDemo();

	void afterFrameSubmitted() override;
	void setUpRenderPass() override;
	void createPipelineLayouts() override;
	void setUpPipelines() override;
	void setUpVulkanBuffers() override;
	void createSecondaryCommandBuffers() override;
	std::vector<ao::vulkan::DrawInCommandBuffer> updateSecondaryCommandBuffers() override; 
	void updateUniformBuffers() override;
	vk::QueueFlags queueFlags() override;
	void createDescriptorSetLayouts() override;
	void createDescriptorPools() override;
	void createDescriptorSets() override;
};

