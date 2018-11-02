#include "ao_shadermodule.h"

ao::vulkan::AOShaderModule::AOShaderModule(AODevice* device) {
	this->device = device;
}

ao::vulkan::AOShaderModule::~AOShaderModule() {
	for (auto& pair : this->shaders) {
		this->device->logical.destroyShaderModule(pair.second->module);
		delete pair.second;
	}
	this->shaders.clear();
}

std::vector<char> ao::vulkan::AOShaderModule::read(std::string filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	std::vector<char> vector;

	// Check file
	if (!file.is_open()) {
		throw ao::core::Exception("Fail to open:" + filename);
	}

	// Get size
	size_t fileSize = static_cast<size_t>(file.tellg());

	// Resize vector
	vector.resize(fileSize);

	// Copy into vector
	file.seekg(0);
	file.read(vector.data(), fileSize);
	
	// Close file
	file.close();

	// Check vector
	if (vector.empty()) {
		throw ao::core::Exception("Fail to copy code into vector");
	}

	return vector;
}

vk::ShaderModule ao::vulkan::AOShaderModule::createModule(const std::vector<char>& code) {
	return this->device->logical.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<const uint32_t*>(code.data())));
}

ao::vulkan::AOShaderModule & ao::vulkan::AOShaderModule::loadShader(std::string filename, vk::ShaderStageFlagBits flag) {
	// Create module
	vk::ShaderModule module = ao::vulkan::AOShaderModule::createModule(ao::vulkan::AOShaderModule::read(filename));

	// Destroy old one
	auto it = this->shaders.find(flag);
	if (it != this->shaders.end()) {
		this->device->logical.destroyShaderModule(this->shaders[flag]->module);
		delete this->shaders[flag];

		this->shaders.erase(it);
	}

	// Add to map
	this->shaders[flag] = new vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), flag, module, "main");

	return *this;
}

std::vector <vk::PipelineShaderStageCreateInfo> ao::vulkan::AOShaderModule::shaderStages() {
	std::vector<vk::PipelineShaderStageCreateInfo> vector(this->shaders.size());

	// Copy into vector
	auto it = this->shaders.begin();
	for (size_t i = 0; i < vector.size(); i++) {
		vector[i] = *(it++)->second;
	}

	return vector;
}
