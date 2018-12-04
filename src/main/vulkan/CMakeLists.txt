file(GLOB_RECURSE AO_VK_SRC "*.cpp")
file(GLOB_RECURSE AO_VK_HEADERS "*.h" "*.hpp")

# Define library
create_library("ao-vulkan" ${CMAKE_CURRENT_SOURCE_DIR} "${Vulkan_LIBRARY};ao-core")

# Re-create sub-directories in Visual Studio
IF (WIN32)
	create_sub_directories(AO_VK_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} "Header Files")
	create_sub_directories(AO_VK_SRC ${CMAKE_CURRENT_SOURCE_DIR} "Source Files")
ENDIF()
