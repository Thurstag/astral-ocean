file(GLOB VK_SRC "*.cpp")
file(GLOB VK_HEADERS "*.h")

# Define library
add_library(vulkan STATIC ${VK_HEADERS} ${VK_SRC})
target_link_libraries(vulkan ${Vulkan_LIBRARY} glfw)
