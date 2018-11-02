#pragma once

#include <ao/vulkan/ao_shadermodule.h>
#include <ao/vulkan/engine_settings.h>
#include <ao/vulkan/glfw_engine.h>
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
	std::vector<Vertex> vertices;

	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer vertexBuffer;

	TriangleDemo(ao::vulkan::EngineSettings settings) : ao::vulkan::GLFWEngine(settings), ao::vulkan::AOEngine(settings) {
		this->vertices = {
			{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
			{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{ {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} }
		};
	};
	virtual ~TriangleDemo();

	void drawCommandBuffer(vk::CommandBuffer& commandBuffer, vk::RenderPassBeginInfo& renderPassInfo, ao::vulkan::WindowSettings& winSettings) override;
	void setUpRenderPass() override;
	void setUpPipeline() override;
	void setUpVertexBuffers() override;

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

