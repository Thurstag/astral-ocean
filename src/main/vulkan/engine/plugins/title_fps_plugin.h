#pragma once

#include <string>

#include "fps_plugin.h"

namespace ao {
	namespace vulkan {
		class TitleFPSPlugin : public FPSPlugin {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="engine">Engine</param>
			TitleFPSPlugin(AOEngine* engine);
			virtual ~TitleFPSPlugin();

			void onInit() override;
			void displayFrameRate(u64 frameRate) override;

		private:
			std::string baseTitle;
		};
	}
}

