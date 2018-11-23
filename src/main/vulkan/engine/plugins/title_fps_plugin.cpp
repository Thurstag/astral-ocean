// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "title_fps_plugin.h"

ao::vulkan::TitleFPSPlugin::TitleFPSPlugin(AOEngine * engine) : ao::vulkan::FPSPlugin(engine) {}

ao::vulkan::TitleFPSPlugin::~TitleFPSPlugin() {}

void ao::vulkan::TitleFPSPlugin::onInit() {
	ao::vulkan::FPSPlugin::onInit();

	// Back-up original title
	this->baseTitle = this->subject->settings().window.name;
}

void ao::vulkan::TitleFPSPlugin::displayFrameRate(u64 frameRate) {
	this->subject->setWindowTitle(fmt::format("{0} - {1}FPS", this->baseTitle, std::to_string(frameRate)));
}
