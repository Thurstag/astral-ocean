#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <vector>

#include <ao/vulkan/engine/wrappers/shadermodule.h>
#include <ao/vulkan/engine/wrappers/buffer.hpp>
#include <ao/vulkan/engine/glfw_engine.h>
#include <ao/vulkan/engine/settings.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions() {
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions;

		attributeDescriptions[0] = vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos));
		attributeDescriptions[1] = vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color));

		return attributeDescriptions;
	}
};

class TriangleDemo : public virtual ao::vulkan::GLFWEngine {
public:
	std::chrono::time_point<std::chrono::system_clock> clock;
	bool clockInit = false;

	std::vector<Vertex> vertices;
	ao::vulkan::Buffer<Vertex*>* buffer;

	TriangleDemo(ao::vulkan::EngineSettings settings) : ao::vulkan::GLFWEngine(settings), ao::vulkan::AOEngine(settings) {
		this->vertices = {
			{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
			{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{ {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} }
		};
	};
	virtual ~TriangleDemo();

	void afterFrameSubmitted() override;
	void setUpRenderPass() override;
	void createPipelineLayouts() override;
	void setUpPipelines() override;
	void setUpVertexBuffers() override;
	void createSecondaryCommandBuffers() override;
	std::vector<ao::vulkan::DrawInCommandBuffer> updateSecondaryCommandBuffers() override;
};

