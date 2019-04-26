// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "shader_module.h"

#include <ao/core/exception/file_not_found.h>
#include <ao/core/utilities/types.h>
#include <fmt/format.h>

ao::vulkan::ShaderModule::ShaderModule(std::shared_ptr<vk::Device> device) : device(device) {}

ao::vulkan::ShaderModule::~ShaderModule() {
    for (auto& [key, value] : this->shaders) {
        this->device->destroyShaderModule(value.module);
    }
}

std::vector<char> ao::vulkan::ShaderModule::load(std::string const& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    // Check file
    if (!file.is_open()) {
        throw ao::core::FileNotFoundException(filename);
    }

    // Copy file into vector
    std::istreambuf_iterator<char> start(file), end;
    std::vector<char> vector(start, end);

    // Close file
    file.close();
    return vector;
}

vk::ShaderModule ao::vulkan::ShaderModule::createModule(std::vector<char> const& code) {
    return this->device->createShaderModule(
        vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<u32 const*>(code.data())));
}

ao::vulkan::ShaderModule& ao::vulkan::ShaderModule::loadShader(vk::ShaderStageFlagBits flag, std::string const& filename) {
    // Create module
    std::vector<char> code = ao::vulkan::ShaderModule::load(filename);
    vk::ShaderModule module = ao::vulkan::ShaderModule::createModule(code);

    // Check code
    if (code.empty()) {
        throw ao::core::Exception("Fail load shader");
    }

    // Lock mutex
    std::lock_guard lock(*this->shaders_mutex);

    // Destroy old one
    auto it = this->shaders.find(flag);
    if (it != this->shaders.end()) {
        this->device->destroyShaderModule(this->shaders[flag].module);

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
