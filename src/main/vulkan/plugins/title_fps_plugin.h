#pragma once

#include <string>

#include "fps_plugin.h"

namespace ao {
	namespace vk {
		class TitleFPSPlugin : public FPSPlugin {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="engine">Engine</param>
			TitleFPSPlugin(AOEngine* engine);
			virtual ~TitleFPSPlugin();

			void onInit() override;
			void displayFrameRate(uint64_t frameRate);

		private:
			std::string baseTitle;
		};
	}
}

