// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <string>

#include <fmt/format.h>

#include "fps_plugin.h"

namespace ao::vulkan {
	class TitleFPSPlugin : public FPSPlugin {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="engine">Engine</param>
		explicit TitleFPSPlugin(AOEngine* engine);
		virtual ~TitleFPSPlugin();

		void onInit() override;
		void displayFrameRate(u64 frameRate) const override;

	private:
		std::string baseTitle;
	};
}

