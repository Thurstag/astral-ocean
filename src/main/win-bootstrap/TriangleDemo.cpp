#include "TriangleDemo.h"

TriangleDemo::~TriangleDemo() {
	delete this->buffer;
}

void TriangleDemo::afterFrameSubmitted() {
	ao::vulkan::GLFWEngine::afterFrameSubmitted();

	if (!this->clockInit) {
		this->clock = std::chrono::system_clock::now();
		this->clockInit = true;

		return;
	}

	// Define rotate axis
	glm::vec3 rotationAxis(0.0f, 0.0f, 1.0f); // ~ Z-axis

	// Delta time
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - this->clock).count() / 1000.0f;
	float angles = glm::half_pi<float>(); // Rotation in 1 second

	for (Vertex& vertice : this->vertices) {
		// To vec4
		glm::vec4 point(vertice.pos.x, vertice.pos.y, 0, 0);

		// Rotate point
		point = glm::rotate(angles * deltaTime, rotationAxis) * point;

		// Update vertice
		vertice.pos = glm::vec2(point.x, point.y);
	}

	// Update vertex buffer
	this->buffer->update(this->vertices.data());

	// Update clock
	this->clock = std::chrono::system_clock::now();
}

void TriangleDemo::setUpRenderPass() {
	std::array<vk::AttachmentDescription, 2> attachments;

	// Color attachment
	attachments[0] = vk::AttachmentDescription(
		vk::AttachmentDescriptionFlags(), this->swapchain->colorFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
	);

	// Depth attachment
	attachments[1] = vk::AttachmentDescription(
		vk::AttachmentDescriptionFlags(), this->device->depthFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal
	);

	vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpassDescription(
		vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics,
		0, nullptr, 1, &colorReference, nullptr, &depthReference
	);

	// Subpass dependencies for layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;

	dependencies[0] = vk::SubpassDependency(
		VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe,
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead,
		vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
		vk::DependencyFlagBits::eByRegion
	);

	dependencies[1] = vk::SubpassDependency(
		0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
		vk::AccessFlagBits::eMemoryRead,
		vk::DependencyFlagBits::eByRegion
	);

	this->renderPass = this->device->logical.createRenderPass(vk::RenderPassCreateInfo(
		vk::RenderPassCreateFlags(), static_cast<uint32_t>(attachments.size()),
		attachments.data(), 1, &subpassDescription, static_cast<uint32_t>(dependencies.size()),
		dependencies.data()
	));
}

void TriangleDemo::createPipelineLayouts() {
	this->pipeline->layouts.resize(1);

	this->pipeline->layouts[0] = this->device->logical.createPipelineLayout(vk::PipelineLayoutCreateInfo());
}

void TriangleDemo::setUpPipelines() {
	// Create shadermodules
	ao::vulkan::ShaderModule module(this->device);

	// Load shaders & get shaderStages
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = module
		.loadShader("vert.spv", vk::ShaderStageFlagBits::eVertex)
		.loadShader("frag.spv", vk::ShaderStageFlagBits::eFragment).shaderStages();

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setLayout(this->pipeline->layouts[0])
		.setRenderPass(this->renderPass);

	// Construct the differnent states making up the pipeline

	// Set pipeline shader stage info
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineCreateInfo.pStages = shaderStages.data();

	// Input assembly state
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

	// Rasterization state
	vk::PipelineRasterizationStateCreateInfo rasterizationState = vk::PipelineRasterizationStateCreateInfo()
		.setPolygonMode(vk::PolygonMode::eFill)
		.setCullMode(vk::CullModeFlagBits::eNone)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setLineWidth(1.0f);

	// Color blend state
	std::array<vk::PipelineColorBlendAttachmentState, 1> blendAttachmentState;
	blendAttachmentState[0].setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo colorBlendState = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(static_cast<uint32_t>(blendAttachmentState.size()))
		.setPAttachments(blendAttachmentState.data());

	// Viewport state
	vk::Viewport viewport(0, 0, static_cast<float>(this->swapchain->currentExtent.width), static_cast<float>(this->swapchain->currentExtent.height), 0, 1);
	vk::Rect2D scissor(vk::Offset2D(), this->swapchain->currentExtent);
	vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);

	// Enable dynamic states
	std::vector<vk::DynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(vk::DynamicState::eViewport);
	dynamicStateEnables.push_back(vk::DynamicState::eScissor);

	vk::PipelineDynamicStateCreateInfo dynamicState(vk::PipelineDynamicStateCreateFlags(), static_cast<uint32_t>(dynamicStateEnables.size()), dynamicStateEnables.data());

	// Depth and stencil state
	vk::PipelineDepthStencilStateCreateInfo depthStencilState = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(VK_TRUE)
		.setDepthWriteEnable(VK_TRUE)
		.setDepthCompareOp(vk::CompareOp::eLessOrEqual)
		.setBack(vk::StencilOpState(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways));
	depthStencilState.setFront(depthStencilState.back);

	// Multi sampling state
	vk::PipelineMultisampleStateCreateInfo multisampleState;

	// Vertex input descriptions 
	// Specifies the vertex input parameters for a pipeline

	// Vertex input binding
	vk::VertexInputBindingDescription vertexInputBinding = vk::VertexInputBindingDescription().setStride(sizeof(Vertex));

	// Inpute attribute bindings
	std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributes;

	vertexInputAttributes[0]
		.setFormat(vk::Format::eR32G32Sfloat)
		.setOffset(offsetof(Vertex, pos));

	vertexInputAttributes[1]
		.setLocation(1)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setOffset(offsetof(Vertex, color));

	// Vertex input state used for pipeline creation
	vk::PipelineVertexInputStateCreateInfo vertexInputState(
		vk::PipelineVertexInputStateCreateFlags(), 1, &vertexInputBinding,
		static_cast<uint32_t>(vertexInputAttributes.size()), vertexInputAttributes.data()
	);

	// Assign the pipeline states to the pipeline creation info structure
	pipelineCreateInfo.setPVertexInputState(&vertexInputState)
		.setPInputAssemblyState(&inputAssemblyState)
		.setPRasterizationState(&rasterizationState)
		.setPColorBlendState(&colorBlendState)
		.setPMultisampleState(&multisampleState)
		.setPViewportState(&viewportState)
		.setPDepthStencilState(&depthStencilState)
		.setRenderPass(renderPass)
		.setPDynamicState(&dynamicState);

	// Create rendering pipeline using the specified states
	this->pipeline->pipelines = this->device->logical.createGraphicsPipelines(this->pipeline->cache, pipelineCreateInfo);
}

void TriangleDemo::setUpVertexBuffers() {
	this->buffer = &(new ao::vulkan::DeviceBuffer<Vertex*>(this->device))->init(sizeof(Vertex) * this->vertices.size(), this->vertices.data());
}

void TriangleDemo::createSecondaryCommandBuffers() {
	this->swapchain->secondaryCommandBuffers = this->device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(this->swapchain->commandPool, vk::CommandBufferLevel::eSecondary, 1));
}

std::vector<ao::vulkan::DrawInCommandBuffer> TriangleDemo::updateSecondaryCommandBuffers() {
	vk::CommandBuffer& commandBuffer = this->swapchain->secondaryCommandBuffers[0];
	std::vector<ao::vulkan::DrawInCommandBuffer> commands;
	vk::Pipeline& pipeline = this->pipeline->pipelines[0];
	vk::Buffer& vertexBuffer = this->buffer->buffer();
	std::vector<Vertex>& vertices = this->vertices;

	commands.push_back([commandBuffer, pipeline, vertexBuffer, vertices](vk::CommandBufferInheritanceInfo& inheritance, std::pair<std::array<vk::ClearValue, 2>, vk::Rect2D>& helpers) {
		vk::Viewport viewPort(0, 0, static_cast<float>(helpers.second.extent.width), static_cast<float>(helpers.second.extent.height), 0, 1);
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setPInheritanceInfo(&inheritance);

		// Begin
		commandBuffer.begin(beginInfo);

		// Set viewport & scissor
		commandBuffer.setViewport(0, viewPort);
		commandBuffer.setScissor(0, helpers.second);

		// Bind pipeline
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		// Draw triangle
		std::array<vk::Buffer, 1> vertexBuffers = { vertexBuffer };
		std::array<vk::DeviceSize, 1> offsets = { 0 };
		commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
		commandBuffer.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

		// End
		commandBuffer.end();

		return commandBuffer;
	});

	return commands;
}

vk::QueueFlags TriangleDemo::queueFlags() {
	// Enable transfer flag
	return ao::vulkan::GLFWEngine::queueFlags() | vk::QueueFlagBits::eTransfer;
}
