#pragma once

#include <vector>

#include <ao/core/utilities/types.h>
#include <GLFW/glfw3.h>

namespace ao {
	namespace vulkan {
		namespace utilities {
			/// <summary>
			/// Method to get monitors
			/// </summary>
			/// <returns>Monitors</returns>
			inline std::vector<GLFWmonitor*> monitors() {
				int count;

				// Get monitors
				GLFWmonitor** monitors = glfwGetMonitors(&count);
				return std::vector<GLFWmonitor*>(monitors, monitors + count);
			}

			/// <summary>
			/// Method to get extensions
			/// </summary>
			/// <returns>Extensions</returns>
			inline std::vector<char const*> extensions() {
				u32 count;

				// Get extensions
				char const** extensions = glfwGetRequiredInstanceExtensions(&count);
				return std::vector<char const*>(extensions, extensions + count);
			}
		}
	}
}
