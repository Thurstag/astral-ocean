#include "title_fps_plugin.h"

ao::vk::TitleFPSPlugin::TitleFPSPlugin(AOEngine * engine) : ao::vk::FPSPlugin(engine) {}

ao::vk::TitleFPSPlugin::~TitleFPSPlugin() {}

void ao::vk::TitleFPSPlugin::onInit() {
	ao::vk::FPSPlugin::onInit();

	// Back-up original title
	this->baseTitle = this->subject->settings().window.name;
}

void ao::vk::TitleFPSPlugin::displayFrameRate(uint64_t frameRate) {
	this->subject->setWindowTitle(this->baseTitle + " - " + std::to_string(frameRate) + "FPS");
}
