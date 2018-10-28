#pragma once

#include <core/plugin.h>
#include <chrono>

#include "../ao_engine.h"

namespace ao {
	namespace vk {
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
			virtual void BeforeDestroy() override;

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

