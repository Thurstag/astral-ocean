#pragma once

#include "GLFW/glfw3.h"

#include "ao_engine.h"

namespace ao {
	namespace vk {
		class GLFWEngine : public virtual AOEngine {
		public:
			GLFWEngine(EngineSettings settings) : AOEngine(settings) {};
			~GLFWEngine() override;

		protected:
			void initWindow() override;
			void freeWindow() override;

			void loop() override;

			std::vector<char const*> instanceExtensions() override;
			
		private:
			GLFWwindow* window;
		};
	}
}
