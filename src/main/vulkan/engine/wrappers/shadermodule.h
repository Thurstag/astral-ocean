// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

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
		/// <param name="_device">Device</param>
		explicit ShaderModule(std::weak_ptr<Device> _device);

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
		ShaderModule& loadShader(std::string const& filename, vk::ShaderStageFlagBits const flag);

		/// <summary>
		/// Method to get shaderStages
		/// </summary>
		/// <returns>shaderStages</returns>
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages() const;

	protected:
		std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo*> shaders;
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
		/// <param name="device">Device</param>
		/// <param name="code">Code</param>
		/// <returns>vk::ShaderModule</returns>
		vk::ShaderModule createModule(std::vector<char> const& code);
	};
}
