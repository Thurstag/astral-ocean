#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include <ao/vulkan/ao_device.h>
#include <ao/core/exception.h>
#include <vulkan/vulkan.hpp>

namespace ao {
	namespace vulkan {
		class AOShaderModule {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			AOShaderModule(AODevice* device);

			/// <summary>
			/// Destructor
			/// </summary>
			~AOShaderModule();

			/// <summary>
			/// Method to load a shader
			/// </summary>
			/// <param name="filename">Filename</param>
			/// <param name="flag">Flag</param>
			/// <returns>AOShaderModule</returns>
			AOShaderModule& loadShader(std::string filename, vk::ShaderStageFlagBits flag);

			/// <summary>
			/// Method to get shaderStages
			/// </summary>
			/// <returns>shaderStages</returns>
			std::vector<vk::PipelineShaderStageCreateInfo> shaderStages();

		private:
			std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo*> shaders;
			AODevice* device;

			/// <summary>
			/// Method to read a shader file
			/// </summary>
			/// <param name="filename">File's name</param>
			/// <returns>Vector</returns>
			std::vector<char> read(std::string filename);

			/// <summary>
			/// Method to create a shader module
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="code">Code</param>
			/// <returns>vk::ShaderModule</returns>
			vk::ShaderModule createModule(const std::vector<char>& code);
		};
	}
}
