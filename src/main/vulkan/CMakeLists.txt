file(GLOB_RECURSE AO_VK_SRC "*.cpp")
file(GLOB_RECURSE AO_VK_HEADERS "*.h" "*.hpp")

# Define library
create_library("ao-vulkan" ${CMAKE_CURRENT_SOURCE_DIR} "${Vulkan_LIBRARY};glfw;${GLM_LIBRARY};ao-core;${CMAKE_THREAD_LIBS_INIT}")

# Copy headers in include directory
set(AO_VK_FILES ${AO_VK_SRC} ${AO_VK_SRC})
file(REMOVE_RECURSE "${AO_INCLUDE_DIR}/ao/vulkan")
file(MAKE_DIRECTORY "${AO_INCLUDE_DIR}/ao/vulkan")
file(COPY "." DESTINATION "${AO_INCLUDE_DIR}/ao/vulkan" 
	FILES_MATCHING REGEX ".*\\.h.*"
	PATTERN "*.dir" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
)

# Re-create sub-directories in Visual Studio
IF (WIN32)
	create_sub_directories(AO_VK_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} "Header Files")
	create_sub_directories(AO_VK_SRC ${CMAKE_CURRENT_SOURCE_DIR} "Source Files")
ENDIF()
