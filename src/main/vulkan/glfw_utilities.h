#pragma once

#include <GLFW/glfw3.h>
#include <vector>

namespace ao {
	namespace glfw {
		namespace utilities {
			/// <summary>
			/// Method to get monitors
			/// </summary>
			/// <returns>Monitors</returns>
			inline std::vector<GLFWmonitor*> getMonitors() {
				int count;

                // Get monitors
				GLFWmonitor** monitors = glfwGetMonitors(&count);
				return std::vector<GLFWmonitor*>(monitors, monitors + count);
			}

			/// <summary>
			/// Method to get extensions
			/// </summary>
			/// <returns>Extensions</returns>
			inline std::vector<char const*> getExtensions() {
				uint32_t count;

                // Get extensions
				char const** extensions = glfwGetRequiredInstanceExtensions(&count);
				return std::vector<char const*>(extensions, extensions + count);
			}
		}
	}
}
