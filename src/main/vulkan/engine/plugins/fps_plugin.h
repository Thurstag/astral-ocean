// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <chrono>

#include <ao/core/plugin.hpp>

#include "../ao_engine.h"

namespace ao {
	namespace vulkan {
		class FPSPlugin : public core::Plugin<AOEngine> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="engine">Engine</param>
			explicit FPSPlugin(AOEngine* engine);
			virtual ~FPSPlugin();

			virtual void onInit() override;
			virtual void onUpdate() override;
			virtual void beforeDestroy() override;

			/// <summary>
			/// Method to display frame rate
			/// </summary>
			/// <param name="frameRate">Frame rate</param>
			virtual void displayFrameRate(u64 frameRate) = 0;

		private:
			std::chrono::time_point<std::chrono::system_clock> clock;
			u64 frameRate;
			bool init = false;
		};
	}
}

