// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "shader_module.h"

ao::vulkan::ShaderModule::ShaderModule(std::weak_ptr<Device> device) : device(device) {}

ao::vulkan::ShaderModule::~ShaderModule() {
    if (auto _device = ao::core::shared(this->device)) {
        for (auto& [key, value] : this->shaders) {
            _device->logical.destroyShaderModule(value.module);
        }
        this->shaders.clear();
    }
}

std::vector<char> ao::vulkan::ShaderModule::read(std::string const& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    // Check file
    if (!file.is_open()) {
        throw ao::core::Exception(fmt::format("Fail to open: {0}", filename));
    }

    // Copy file into vector
    std::istreambuf_iterator<char> start(file), end;
    std::vector<char> vector(start, end);

    // Close file
    file.close();

    // Check vector
    if (vector.empty()) {
        throw ao::core::Exception("Fail to copy code into vector");
    }

    return vector;
}

vk::ShaderModule ao::vulkan::ShaderModule::createModule(std::vector<char> const& code) {
    return ao::core::shared(this->device)
        ->logical.createShaderModule(
            vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<u32 const*>(code.data())));
}

ao::vulkan::ShaderModule& ao::vulkan::ShaderModule::loadShader(vk::ShaderStageFlagBits flag, std::string const& filename) {
    auto _device = ao::core::shared(this->device);

    // Create module
    std::vector<char> code = ao::vulkan::ShaderModule::read(filename);
    vk::ShaderModule module = ao::vulkan::ShaderModule::createModule(code);

    // Destroy old one
    auto it = this->shaders.find(flag);
    if (it != this->shaders.end()) {
        _device->logical.destroyShaderModule(this->shaders[flag].module);

        this->shaders.erase(it);
    }

    // Add to map
    this->shaders[flag] = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), flag, module, "main");
    return *this;
}

std::vector<vk::PipelineShaderStageCreateInfo> ao::vulkan::ShaderModule::shaderStages() const {
    std::vector<vk::PipelineShaderStageCreateInfo> vector(this->shaders.size());

    // Copy into vector
    size_t i = 0;
    for (auto [key, value] : this->shaders) {
        vector[i++] = value;
    }

    return vector;
}