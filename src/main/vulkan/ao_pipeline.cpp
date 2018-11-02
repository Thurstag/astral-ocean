#include "ao_pipeline.h"

ao::vulkan::AOPipeline::AOPipeline(AODevice * device) {
	this->device = device;
}

ao::vulkan::AOPipeline::~AOPipeline() {
	this->device->logical.destroyPipeline(this->pipeline);
	this->device->logical.destroyPipelineLayout(this->layout);
	this->device->logical.destroyPipelineCache(this->cache);
}
