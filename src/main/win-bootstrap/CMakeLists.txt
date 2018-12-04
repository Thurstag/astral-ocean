SET(MAIN_CPP main.cpp)

# Include *.cpp & vk *.h
file(GLOB_RECURSE SOURCE *.cpp *.h *.hpp)

set(PATH_TO_SUBMODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../submodules")

# GLM library
find_package(GLM REQUIRED)
include_directories("${GLM_INCLUDE_DIR}")

# GFLW library
add_subdirectory("${PATH_TO_SUBMODULES}/glfw" "${CMAKE_CURRENT_SOURCE_DIR}/glfw")
include_directories("${SUBMODULES_DIR}/glfw/include")

# STB library
include_directories("${SUBMODULES_DIR}/stb/")

# Define executable
add_executable("win-bootstrap" ${MAIN_CPP} ${SOURCE})
set_target_properties("win-bootstrap" PROPERTIES  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bootstraps")

# Import libraries
target_link_libraries("win-bootstrap" "ao-vulkan;glfw;${GLM_LIBRARY}")
