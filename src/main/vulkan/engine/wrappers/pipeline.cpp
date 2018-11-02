#include "pipeline.h"

ao::vulkan::Pipeline::Pipeline(Device * device) {
	this->device = device;
}

ao::vulkan::Pipeline::~Pipeline() {
	this->device->logical.destroyPipeline(this->graphics);
	this->device->logical.destroyPipelineLayout(this->layout);
	this->device->logical.destroyPipelineCache(this->cache);
}
