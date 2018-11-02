#include "title_fps_plugin.h"

ao::vulkan::TitleFPSPlugin::TitleFPSPlugin(AOEngine * engine) : ao::vulkan::FPSPlugin(engine) {}

ao::vulkan::TitleFPSPlugin::~TitleFPSPlugin() {}

void ao::vulkan::TitleFPSPlugin::onInit() {
	ao::vulkan::FPSPlugin::onInit();

	// Back-up original title
	this->baseTitle = this->subject->settings().window.name;
}

void ao::vulkan::TitleFPSPlugin::displayFrameRate(uint64_t frameRate) {
	this->subject->setWindowTitle(this->baseTitle + " - " + std::to_string(frameRate) + "FPS");
}
