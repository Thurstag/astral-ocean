#pragma once

#include "GLFW/glfw3.h"

#include "engine.h"

namespace ao {
	namespace vk {
		class GLFWEngine : public virtual Engine {
		public:
			GLFWEngine(EngineSettings settings) : Engine(settings) {};
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
