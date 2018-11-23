#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <vector>

#include <ao/vulkan/engine/wrappers/buffers/tuple/staging_buffer.hpp>
#include <ao/vulkan/engine/wrappers/buffers/array/basic_buffer.hpp>
#include <ao/vulkan/engine/wrappers/shadermodule.h>
#include <ao/vulkan/engine/glfw_engine.h>
#include <ao/vulkan/engine/settings.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "vertex.hpp"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class RectangleDemo : public virtual ao::vulkan::GLFWEngine {
public:
	std::chrono::time_point<std::chrono::system_clock> clock;
	bool clockInit = false;

	std::vector<Vertex> vertices;
	std::vector<u16> indices;

	std::unique_ptr<ao::vulkan::TupleBuffer<Vertex, u16>> rectangleBuffer;
	std::unique_ptr<ao::vulkan::DynamicArrayBuffer<UniformBufferObject>> uniformBuffer;

	std::vector<UniformBufferObject> _uniformBuffers;

	explicit RectangleDemo(ao::vulkan::EngineSettings settings) : 
		ao::vulkan::GLFWEngine(settings), 
		ao::vulkan::AOEngine(settings),
		vertices({
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		}),
		indices({ 0, 1, 2, 2, 3, 0 }) {
	};
	virtual ~RectangleDemo();

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

