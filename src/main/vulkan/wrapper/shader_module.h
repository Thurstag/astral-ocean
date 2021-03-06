// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <ao/core/exception/exception.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /**
     * @brief vk::ShaderModule wrapper
     *
     */
    class ShaderModule {
       public:
        /**
         * @brief Construct a new ShaderModule object
         *
         * @param device Device
         */
        explicit ShaderModule(std::shared_ptr<vk::Device> device);

        /**
         * @brief Destroy the ShaderModule object
         *
         */
        ~ShaderModule();

        /**
         * @brief Load a shader
         *
         * @param flag Flag
         * @param filename File's name
         * @return ShaderModule& ShaderModule
         */
        ShaderModule& loadShader(vk::ShaderStageFlagBits flag, std::string const& filename);

        /**
         * @brief Shader stages
         *
         * @return std::vector<vk::PipelineShaderStageCreateInfo> Shader stages
         */
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages() const;

       protected:
        std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo> shaders;
        std::unique_ptr<std::mutex> shaders_mutex;
        std::shared_ptr<vk::Device> device;

        /**
         * @brief Load a file into a vector<char>
         *
         * @param filename File's name
         * @return std::vector<char> File's content
         */
        std::vector<char> load(std::string const& filename);

        /**
         * @brief Create a ShaderModule
         *
         * @param code Code
         * @return vk::ShaderModule ShaderModule
         */
        vk::ShaderModule createModule(std::vector<char> const& code);
    };
}  // namespace ao::vulkan
