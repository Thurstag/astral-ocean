// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/map_container.hpp>
#include <ao/core/utilities/pointers.h>
#include <vulkan/vulkan.hpp>

#include "../wrappers/device.h"

namespace ao::vulkan {
	struct CommandBufferData {
		std::vector<vk::CommandBuffer> buffers;
		vk::CommandPool pool;

		/// <summary>
		/// Constructor
		/// </summary>
		CommandBufferData() = default;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="_buffers">Buffers</param>
		/// <param name="_pool">Pool</param>
		explicit CommandBufferData(std::vector<vk::CommandBuffer> const& _buffers, vk::CommandPool const& _pool) : buffers(_buffers), pool(_pool) {}
	};

	class CommandBufferContainer : public core::MapContainer<std::string, CommandBufferData> {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="_device">Device</param>
		explicit CommandBufferContainer(std::weak_ptr<Device> _device);

		/// <summary>
		/// Destructor
		/// </summary>
		~CommandBufferContainer();

		virtual void clear() override;

	protected:
		std::weak_ptr<Device> device;
	};
}
