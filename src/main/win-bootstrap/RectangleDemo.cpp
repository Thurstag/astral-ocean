#include "RectangleDemo.h"

RectangleDemo::~RectangleDemo() {
	this->rectangleBuffer.reset();
	this->uniformBuffer.reset();
}

void RectangleDemo::setUpRenderPass() {
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
		vk::RenderPassCreateFlags(), static_cast<u32>(attachments.size()),
		attachments.data(), 1, &subpassDescription, static_cast<u32>(dependencies.size()),
		dependencies.data()
	));
}

void RectangleDemo::createPipelineLayouts() {
	this->pipeline->layouts.resize(1);

	this->pipeline->layouts.front() = this->device->logical.createPipelineLayout(
		vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), static_cast<u32>(this->descriptorSetLayouts.size()), this->descriptorSetLayouts.data())
	);
}

void RectangleDemo::setUpPipelines() {
	// Create shadermodules
	ao::vulkan::ShaderModule module(this->device);

	// Load shaders & get shaderStages
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = module
		.loadShader("rec-vert.spv", vk::ShaderStageFlagBits::eVertex)
		.loadShader("rec-frag.spv", vk::ShaderStageFlagBits::eFragment).shaderStages();

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setLayout(this->pipeline->layouts[0])
		.setRenderPass(this->renderPass);

	// Construct the differnent states making up the pipeline

	// Set pipeline shader stage info
	pipelineCreateInfo.stageCount = static_cast<u32>(shaderStages.size());
	pipelineCreateInfo.pStages = shaderStages.data();

	// Input assembly state
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

	// Rasterization state
	vk::PipelineRasterizationStateCreateInfo rasterizationState = vk::PipelineRasterizationStateCreateInfo()
		.setPolygonMode(vk::PolygonMode::eFill)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setLineWidth(1.0f);

	// Color blend state
	std::array<vk::PipelineColorBlendAttachmentState, 1> blendAttachmentState;
	blendAttachmentState[0].setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo colorBlendState = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(static_cast<u32>(blendAttachmentState.size()))
		.setPAttachments(blendAttachmentState.data());

	// Viewport state
	vk::Viewport viewport(0, 0, static_cast<float>(this->swapchain->currentExtent.width), static_cast<float>(this->swapchain->currentExtent.height), 0, 1);
	vk::Rect2D scissor(vk::Offset2D(), this->swapchain->currentExtent);
	vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor);

	// Enable dynamic states
	std::vector<vk::DynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(vk::DynamicState::eViewport);
	dynamicStateEnables.push_back(vk::DynamicState::eScissor);

	vk::PipelineDynamicStateCreateInfo dynamicState(vk::PipelineDynamicStateCreateFlags(), static_cast<u32>(dynamicStateEnables.size()), dynamicStateEnables.data());

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
		static_cast<u32>(vertexInputAttributes.size()), vertexInputAttributes.data()
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

void RectangleDemo::setUpVulkanBuffers() {
	// Create vertices & indices
	this->rectangleBuffer = std::unique_ptr<ao::vulkan::TupleBuffer<Vertex, u16>>(
		(new ao::vulkan::StagingTupleBuffer<Vertex, u16>(this->device))
		->init({ sizeof(Vertex) * this->vertices.size(), sizeof(u16) * this->indices.size() })
		->update(this->vertices.data(), this->indices.data())
	);

	this->uniformBuffer = std::unique_ptr<ao::vulkan::TupleBuffer<UniformBufferObject>>(
		(new ao::vulkan::BasicTupleBuffer<UniformBufferObject>(this->device))
		->init(vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		{ sizeof(UniformBufferObject) })
	);

	// Update buffer
	this->_uniformBuffer = UniformBufferObject();
}

void RectangleDemo::createSecondaryCommandBuffers() {
	this->swapchain->secondaryCommandBuffers = this->device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(this->swapchain->commandPool, vk::CommandBufferLevel::eSecondary, 1));
}

std::vector<ao::vulkan::DrawInCommandBuffer> RectangleDemo::updateSecondaryCommandBuffers() {
	std::vector<ao::vulkan::DrawInCommandBuffer> commands;

	vk::CommandBuffer& commandBuffer = this->swapchain->secondaryCommandBuffers[0];
	std::vector<vk::DescriptorSet>& sets = this->descriptorSets;
	vk::Pipeline& pipeline = this->pipeline->pipelines[0];
	vk::PipelineLayout& pipelineLayout = this->pipeline->layouts[0];
	ao::vulkan::TupleBuffer<Vertex, u16>* rectangle = this->rectangleBuffer.get();
	std::vector<Vertex>& vertices = this->vertices;
	std::vector<u16>& indices = this->indices;

	commands.push_back([commandBuffer, pipeline, rectangle, vertices, indices, pipelineLayout, sets]
	(int frameIndex, vk::CommandBufferInheritanceInfo& inheritance, std::pair<std::array<vk::ClearValue, 2>, vk::Rect2D>& helpers) {
		vk::Viewport viewPort(0, 0, static_cast<float>(helpers.second.extent.width), static_cast<float>(helpers.second.extent.height), 0, 1);
		vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue).setPInheritanceInfo(&inheritance);

		// Draw in command
		commandBuffer.begin(beginInfo);
		{
			// Set viewport & scissor
			commandBuffer.setViewport(0, viewPort);
			commandBuffer.setScissor(0, helpers.second);

			// Bind pipeline
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

			// Draw rectangle
			commandBuffer.bindVertexBuffers(0, rectangle->buffer(), { 0 });
			commandBuffer.bindIndexBuffer(rectangle->buffer(), rectangle->offset(1), vk::IndexType::eUint16);
			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, sets.front(), {});

			commandBuffer.drawIndexed(static_cast<u32>(indices.size()), 1, 0, 0, 0);
		}
		commandBuffer.end();

		return commandBuffer;
	});

	return commands;
}

void RectangleDemo::updateUniformBuffers() {
	if (!this->clockInit) {
		this->clock = std::chrono::system_clock::now();
		this->clockInit = true;

		return;
	}

	// Delta time
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::system_clock::now() - this->clock).count();

	// Update uniform buffer
	this->_uniformBuffer.model = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	this->_uniformBuffer.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	this->_uniformBuffer.proj = glm::perspective(glm::radians(45.0f), this->swapchain->currentExtent.width / (float)this->swapchain->currentExtent.height, 0.1f, 10.0f);
	this->_uniformBuffer.proj[1][1] *= -1; // Adapt for vulkan

	// Update buffer
	this->uniformBuffer->update(&this->_uniformBuffer);
}

vk::QueueFlags RectangleDemo::queueFlags() {
	// Enable transfer flag
	return ao::vulkan::GLFWEngine::queueFlags() | vk::QueueFlagBits::eTransfer;
}

void RectangleDemo::createDescriptorSetLayouts() {
	// Create binding
	vk::DescriptorSetLayoutBinding binding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);

	// Create info
	vk::DescriptorSetLayoutCreateInfo createInfo(vk::DescriptorSetLayoutCreateFlags(), 1, &binding);

	// Create layout
	this->descriptorSetLayouts.push_back(this->device->logical.createDescriptorSetLayout(createInfo));
}

void RectangleDemo::createDescriptorPools() {
	vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, 1);

	// Create pool
	this->descriptorPools.push_back(this->device->logical.createDescriptorPool(
		vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1, 1, &poolSize)
	));
}

void RectangleDemo::createDescriptorSets() {
	vk::DescriptorSetAllocateInfo allocateInfo(this->descriptorPools.front(), 1, this->descriptorSetLayouts.data());

	// Create sets
	this->descriptorSets = this->device->logical.allocateDescriptorSets(allocateInfo);

	// Configure
	vk::DescriptorBufferInfo bufferInfo(this->uniformBuffer->buffer(), 0, sizeof(UniformBufferObject));
	this->device->logical.updateDescriptorSets(vk::WriteDescriptorSet(this->descriptorSets.front(), 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo), {});
}
