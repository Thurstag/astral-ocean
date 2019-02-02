// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <ao/core/exception/exception.h>
#include <ao/core/utilities/pointers.h>
#include <vulkan/vulkan.hpp>

#include "device.h"

namespace ao::vulkan {
    /// <summary>
    /// Wrapper for vulkan shaders
    /// </summary>
    class ShaderModule {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        explicit ShaderModule(std::weak_ptr<Device> device);

        /// <summary>
        /// Destructor
        /// </summary>
        ~ShaderModule();

        /// <summary>
        /// Method to load a shader
        /// </summary>
        /// <param name="flag">Flag</param>
        /// <param name="filename">Filename</param>
        /// <returns>ShaderModule</returns>
        ShaderModule& loadShader(vk::ShaderStageFlagBits flag, std::string const& filename);

        /// <summary>
        /// Method to get shaderStages
        /// </summary>
        /// <returns>shaderStages</returns>
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages() const;

       protected:
        std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo> shaders;
        std::weak_ptr<Device> device;

        /// <summary>
        /// Method to read a shader file
        /// </summary>
        /// <param name="filename">File's name</param>
        /// <returns>Vector</returns>
        std::vector<char> read(std::string const& filename);

        /// <summary>
        /// Method to create a shader module
        /// </summary>
        /// <param name="code">Code</param>
        /// <returns>vk::ShaderModule</returns>
        vk::ShaderModule createModule(std::vector<char> const& code);
    };
}  // namespace ao::vulkan
