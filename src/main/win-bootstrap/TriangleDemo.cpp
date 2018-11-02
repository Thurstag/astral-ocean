#include "TriangleDemo.h"

TriangleDemo::~TriangleDemo() {
	this->device->logical.destroyBuffer(this->vertexBuffer);
	this->device->logical.freeMemory(this->vertexBufferMemory);
}

void TriangleDemo::drawCommandBuffer(vk::CommandBuffer& commandBuffer, vk::RenderPassBeginInfo & renderPassInfo, ao::vulkan::WindowSettings & winSettings) {
	// Begin
	commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));

	// Begin render pass
	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	// Bind pipeline
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->pipeline->graphics);

	// Draw triangle
	std::array<vk::Buffer, 1> vertexBuffers = { vertexBuffer };
	std::array<vk::DeviceSize, 1> offsets = { 0 };
	commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
	commandBuffer.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

	// End
	commandBuffer.endRenderPass();
	commandBuffer.end();
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

void TriangleDemo::setUpPipeline() {
	// Create shadermodules
	ao::vulkan::ShaderModule module(this->device);

	// Load shaders & get shaderStages
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = module
		.loadShader("vert.spv", vk::ShaderStageFlagBits::eVertex)
		.loadShader("frag.spv", vk::ShaderStageFlagBits::eFragment).shaderStages();

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setLayout(this->pipeline->layout)
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
	this->pipeline->graphics = this->device->logical.createGraphicsPipelines(this->pipeline->cache, pipelineCreateInfo)[0];
}

void TriangleDemo::setUpVertexBuffers() {
	// TODO: Use staging buffers

	// Create buffer
	this->vertexBuffer = this->device->logical.createBuffer(vk::BufferCreateInfo(
		vk::BufferCreateFlags(), sizeof(this->vertices) * this->vertices.size(),
		vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive)
	);

	// Get memory requirements
	vk::MemoryRequirements memRequirements = this->device->logical.getBufferMemoryRequirements(this->vertexBuffer);

	// Allocate memory
	this->vertexBufferMemory = this->device->logical.allocateMemory(vk::MemoryAllocateInfo(
		memRequirements.size,
		this->device->memoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
	));

	void* data;

	// Bind objects
	vkBindBufferMemory(this->device->logical, this->vertexBuffer, this->vertexBufferMemory, 0);
	vkMapMemory(this->device->logical, this->vertexBufferMemory, 0, sizeof(this->vertices) * this->vertices.size(), 0, &data);

	// Copy vertices into buffer
	memcpy(data, this->vertices.data(), sizeof(this->vertices) * this->vertices.size());
	vkUnmapMemory(this->device->logical, this->vertexBufferMemory);
}
