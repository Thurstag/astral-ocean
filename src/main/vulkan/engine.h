#pragma once

#include "vulkan/vulkan.h"

namespace ao {
	namespace vk {
		class Engine {
		public:
			Engine();
			~Engine();

			virtual void run();
		};
	}
}
