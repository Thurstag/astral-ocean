#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glfw_utilities.h"
#include "ao_engine.h"

namespace ao {
	namespace vk {
		class GLFWEngine : public virtual AOEngine {
		public:
			GLFWEngine(EngineSettings settings) : AOEngine(settings) {};
			~GLFWEngine() override;

		protected:
			void initWindow() override;
			void initSurface(VkSurfaceKHR& surface) override;
			void freeWindow() override;
			bool isIconified() override;

			bool loopingCondition() override;
			void onLoopIteration() override;

			std::vector<char const*> instanceExtensions() override;
			
		private:
			GLFWwindow* window;
		};
	}
}
