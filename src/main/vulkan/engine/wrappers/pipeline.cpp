#include "pipeline.h"

ao::vulkan::Pipeline::Pipeline(std::weak_ptr<ao::vulkan::Device> _device) : device(_device) {}

ao::vulkan::Pipeline::~Pipeline() {
	if (auto _device = ao::core::get(this->device)) {
		for (auto& pipeline : this->pipelines) {
			_device->logical.destroyPipeline(pipeline);
		}
		this->pipelines.clear();

		for (auto& layout : this->layouts) {
			_device->logical.destroyPipelineLayout(layout);
		}
		this->layouts.clear();

		_device->logical.destroyPipelineCache(this->cache);
	}
}
