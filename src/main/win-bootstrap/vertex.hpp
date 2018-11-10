#pragma once

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
