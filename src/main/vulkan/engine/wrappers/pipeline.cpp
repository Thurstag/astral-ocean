#include "pipeline.h"

ao::vulkan::Pipeline::Pipeline(Device * device) {
	this->device = device;
}

ao::vulkan::Pipeline::~Pipeline() {
	for (vk::Pipeline& pipeline : this->pipelines) {
		this->device->logical.destroyPipeline(pipeline);
	}
	this->pipelines.clear();

	for (vk::PipelineLayout& layout : this->layouts) {
		this->device->logical.destroyPipelineLayout(layout);
	}
	this->layouts.clear();

	this->device->logical.destroyPipelineCache(this->cache);
}
