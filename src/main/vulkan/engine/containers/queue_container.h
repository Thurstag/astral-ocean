// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/map_container.hpp>
#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao {
	namespace vulkan {
		struct QueueData {
			vk::QueueFamilyProperties properties;
			vk::Queue queue;
			u32 index;

			/// <summary>
			/// Constructor
			/// </summary>
			QueueData() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_queue">Queue</param>
			/// <param name="_index">Index</param>
			/// <param name="_properties">Properties</param>
			explicit QueueData(vk::Queue _queue, u32 _index, vk::QueueFamilyProperties _properties) : queue(_queue), index(_index), properties(_properties) {}
		};

		class QueueContainer : public core::MapContainer<vk::QueueFlagBits, QueueData> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			QueueContainer() = default;

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~QueueContainer() = default;
		};
	}
}

