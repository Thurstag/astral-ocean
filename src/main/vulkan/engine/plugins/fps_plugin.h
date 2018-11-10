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
			FPSPlugin(AOEngine* engine);
			virtual ~FPSPlugin();

			virtual void onInit() override;
			virtual void onUpdate() override;
			virtual void beforeDestroy() override;

			/// <summary>
			/// Method to display frame rate
			/// </summary>
			/// <param name="frameRate">Frame rate</param>
			virtual void displayFrameRate(uint64_t frameRate) = 0;

		private:
			std::chrono::time_point<std::chrono::system_clock> clock;
			uint64_t frameRate;
			bool init = false;
		};
	}
}
