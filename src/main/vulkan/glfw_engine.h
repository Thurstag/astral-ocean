#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glfw_utilities.h"
#include "ao_engine.h"

namespace ao {
	namespace vulkan {
		class GLFWEngine : public virtual AOEngine {
		public:
			GLFWEngine(EngineSettings settings) : AOEngine(settings) {};
			virtual ~GLFWEngine();

			void setWindowTitle(std::string title) override;
		protected:
			void initWindow() override;
			void initSurface(vk::SurfaceKHR& surface) override;
			void freeWindow() override;
			bool isIconified() override;

			bool loopingCondition() override;
			void onLoopIteration() override;
			void waitMaximized() override;

			std::vector<char const*> instanceExtensions() override;
			
		private:
			GLFWwindow* window;
		};
	}
}
