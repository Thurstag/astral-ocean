#include "fps_plugin.h"

ao::vulkan::FPSPlugin::FPSPlugin(AOEngine * engine) : ao::core::Plugin<AOEngine>(engine) {}

ao::vulkan::FPSPlugin::~FPSPlugin() {}

void ao::vulkan::FPSPlugin::onInit() {}

void ao::vulkan::FPSPlugin::onUpdate() {
	if (!this->init) {
		this->clock = std::chrono::system_clock::now();
		this->init = true;
		this->frameRate = 0;
		return;
	}

	// Increment frame rate
	this->frameRate++;

	// Check duration between now and clock
	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - this->clock).count() > 0) {
		this->displayFrameRate(this->frameRate);

		// Reset clock & count
		this->clock = std::chrono::system_clock::now();
		this->frameRate = 0;
	}
}

void ao::vulkan::FPSPlugin::beforeDestroy() {}
