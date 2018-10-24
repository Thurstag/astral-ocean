cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)

# Define variables
set(PROJECT_NAME "astral-ocean")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(DEPENDENCIES
	Vulkan
	Threads
	Boost
)

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

# Include headers
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR})
include_directories("src/main/vulkan")

# Include sub-projects
add_subdirectory("src/main/vulkan")
add_subdirectory("src/main/bootstrap")