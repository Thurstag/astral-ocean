#include "shadermodule.h"

ao::vulkan::ShaderModule::ShaderModule(Device* _device) : device(_device) {}

ao::vulkan::ShaderModule::~ShaderModule() {
	for (auto& pair : this->shaders) {
		this->device->logical.destroyShaderModule(pair.second->module);
		delete pair.second;
	}
	this->shaders.clear();
}

std::vector<char> ao::vulkan::ShaderModule::read(std::string filename) {
	// Open file and go to the end
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	std::vector<char> vector;

	// Check file
	if (!file.is_open()) {
		throw ao::core::Exception("Fail to open: " + filename);
	}

	// Get size
	size_t fileSize = static_cast<size_t>(file.tellg());

	// Resize vector
	vector.resize(fileSize);

	// Rollback to start & copy into vector
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

vk::ShaderModule ao::vulkan::ShaderModule::createModule(const std::vector<char>& code) {
	return this->device->logical.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<const u32*>(code.data())));
}

ao::vulkan::ShaderModule & ao::vulkan::ShaderModule::loadShader(std::string filename, vk::ShaderStageFlagBits flag) {
	// Create module
	std::vector<char> code = ao::vulkan::ShaderModule::read(filename);
	vk::ShaderModule module = ao::vulkan::ShaderModule::createModule(code);

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

std::vector <vk::PipelineShaderStageCreateInfo> ao::vulkan::ShaderModule::shaderStages() {
	std::vector<vk::PipelineShaderStageCreateInfo> vector(this->shaders.size());

	// Copy into vector
	auto it = this->shaders.begin();
	for (size_t i = 0; i < vector.size(); i++) {
		vector[i] = *(it++)->second;
	}

	return vector;
}
