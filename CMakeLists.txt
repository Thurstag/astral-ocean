cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)

# Define variables
set(PROJECT_NAME "astral-ocean")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Force c++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("${PROJECT_NAME}")

# Try to find vulkan lib
if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	message(STATUS "Using module to find Vulkan library")
	find_package(Vulkan)
endif()

# Try to find thread lib
find_package(Threads REQUIRED)


# Check vulkan lib
IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
	message(STATUS "Vulkan library found: '${Vulkan_LIBRARY}'")
ENDIF()

# Include headers
include_directories(${Vulkan_INCLUDE_DIR})
include_directories("src/main/vulkan")

# Include sub-projects
add_subdirectory("src/main/vulkan")
add_subdirectory("src/main/bootstrap")