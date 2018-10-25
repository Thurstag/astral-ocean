cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)

# Define variables
set(PROJECT_NAME "astral-ocean")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(SUBMODULES_DIR "${CMAKE_BINARY_DIR}/submodules")
set(GLFW_INCLUDE_DIR "${SUBMODULES_DIR}/glfw/include")
set(LOG4CPP_INCLUDE_DIR "${SUBMODULES_DIR}/log4cpp/include")
set(DEPENDENCIES
	Vulkan
	Threads
	Boost
)
set(SUBMODULES
	glfw
	log4cpp
)

# Disable GLFW unwanted builds
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Compilation options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Force c++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("${PROJECT_NAME}")

# Try to find libraries
if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	foreach(DEPENDENCY ${DEPENDENCIES})
		message("Using module to find ${DEPENDENCY} library")
		find_package(${DEPENDENCY})
	endforeach(DEPENDENCY)
endif()

# Check all libs
foreach(DEPENDENCY ${DEPENDENCIES})
	IF(NOT ${${DEPENDENCY}_FOUND})
		message(FATAL_ERROR "Could not find ${DEPENDENCY} library!")
	ELSE()
		set(LIB_PATH "${${DEPENDENCY}_LIBRARY}")
		IF(LIB_PATH STREQUAL "")
			message("${DEPENDENCY} library found")
		ELSE()
			message("${DEPENDENCY} library found: '${${DEPENDENCY}_LIBRARY}'")
		ENDIF()
	ENDIF()
endforeach(DEPENDENCY)

# Add submodules
foreach(SUBMODULE ${SUBMODULES})
	message("Include ${SUBMODULE} submodule")
	add_subdirectory("${SUBMODULES_DIR}/${SUBMODULE}")
endforeach(SUBMODULE)

# Include headers
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${LOG4CPP_INCLUDE_DIR})
include_directories("src/main")

# Include sub-projects
add_subdirectory("src/main/vulkan")
add_subdirectory("src/main/core")
add_subdirectory("src/main/win-bootstrap")
