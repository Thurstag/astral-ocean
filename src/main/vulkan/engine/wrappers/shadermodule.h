#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include <ao/core/exception/exception.h>
#include <vulkan/vulkan.hpp>

#include "device.h"

namespace ao {
	namespace vulkan {
		/// <summary>
		/// Wrapper for vulkan shaders
		/// </summary>
		class ShaderModule {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_device">Device</param>
			ShaderModule(Device* _device);

			/// <summary>
			/// Destructor
			/// </summary>
			~ShaderModule();

			/// <summary>
			/// Method to load a shader
			/// </summary>
			/// <param name="filename">Filename</param>
			/// <param name="flag">Flag</param>
			/// <returns>ShaderModule</returns>
			ShaderModule& loadShader(std::string filename, vk::ShaderStageFlagBits flag);

			/// <summary>
			/// Method to get shaderStages
			/// </summary>
			/// <returns>shaderStages</returns>
			std::vector<vk::PipelineShaderStageCreateInfo> shaderStages();

		protected:
			std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo*> shaders;
			Device* const device;

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
