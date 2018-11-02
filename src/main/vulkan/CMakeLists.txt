file(GLOB_RECURSE AO_VK_SRC "*.cpp")
file(GLOB_RECURSE AO_VK_HEADERS "*.h")

# Define library
add_library("ao-vulkan" ${AO_VK_SRC} ${AO_VK_HEADERS})
target_link_libraries("ao-vulkan" ${Vulkan_LIBRARY} glfw ${GLM_LIBRARY} "ao-core")

# Copy headers in include directory
file(REMOVE_RECURSE "${AO_INCLUDE_DIR}/ao/vulkan")
file(MAKE_DIRECTORY "${AO_INCLUDE_DIR}/ao/vulkan")
file(COPY "." DESTINATION "${AO_INCLUDE_DIR}/ao/vulkan" 
	FILES_MATCHING PATTERN "*.h"
	PATTERN "*.dir" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
)
