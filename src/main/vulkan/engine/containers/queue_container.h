#pragma once

#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

namespace ao {
	namespace vulkan {
		struct QueueData {
			vk::Queue queue;
			uint32_t index;
			vk::QueueFamilyProperties properties;

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
			explicit QueueData(vk::Queue _queue, uint32_t _index, vk::QueueFamilyProperties _properties) : queue(_queue), index(_index), properties(_properties) {}
		};

		class QueueContainer : public core::MapContainer<vk::QueueFlagBits, QueueData> {
		public:
			QueueContainer() = default;
			virtual ~QueueContainer() = default;
		};
	}
}

