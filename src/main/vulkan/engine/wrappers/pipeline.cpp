#include "pipeline.h"

ao::vulkan::Pipeline::Pipeline(Device * _device) : device(_device) {

}

ao::vulkan::Pipeline::~Pipeline() {
	for (auto& pipeline : this->pipelines) {
		this->device->logical.destroyPipeline(pipeline);
	}
	this->pipelines.clear();

	for (auto& layout : this->layouts) {
		this->device->logical.destroyPipelineLayout(layout);
	}
	this->layouts.clear();

	this->device->logical.destroyPipelineCache(this->cache);
}
