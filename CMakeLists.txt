cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Define variables
set(PROJECT_NAME "astral-ocean")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(SUBMODULES_DIR "${CMAKE_BINARY_DIR}/submodules")
set(GLFW_INCLUDE_DIR "${SUBMODULES_DIR}/glfw/include")
set(LOG4CPP_INCLUDE_DIR "${SUBMODULES_DIR}/log4cpp/include")
set(AO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(DEPENDENCIES
	Vulkan
	Threads
	Boost
	GLM
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
IF (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /O2") 
  
  # '/EHsc' Fix issues with log4cpp compilation
  # TODO: Search if there is an issue on other operating systems
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF()

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Force c++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define project name
project("${PROJECT_NAME}" VERSION 0.0.0)

# Try to find libraries
IF (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	FOREACH(DEPENDENCY ${DEPENDENCIES})
		message("Using module to find ${DEPENDENCY} library")
		find_package(${DEPENDENCY})
	ENDFOREACH(DEPENDENCY)
ENDIF()

# Check all libs
FOREACH(DEPENDENCY ${DEPENDENCIES})
	IF(NOT ${${DEPENDENCY}_FOUND})
		message(FATAL_ERROR "Could not find ${DEPENDENCY} library!")
	ELSE()
		set(LIB_PATH "${${DEPENDENCY}_INCLUDE_DIR}")
		IF(LIB_PATH STREQUAL "")
			message("${DEPENDENCY} library found")
		ELSE()
			message("${DEPENDENCY} library found: '${LIB_PATH}', auto-include it")
			include_directories(${LIB_PATH})
		ENDIF()
	ENDIF()
ENDFOREACH(DEPENDENCY)

# Add submodules
FOREACH(SUBMODULE ${SUBMODULES})
	message("Include ${SUBMODULE} submodule")
	add_subdirectory("${SUBMODULES_DIR}/${SUBMODULE}")
ENDFOREACH(SUBMODULE)

# Force folder creation
set_target_properties(log4cpp PROPERTIES FOLDER "LOG4C++")

# Define a function that can re-create sub-directories in visual studio
function(create_sub_directories FILE_LIST CURRENT_DIRECTORY BASE_DIRECTORY)
	FOREACH(SOURCE_FILE IN LISTS ${FILE_LIST})
		# Get relative path
		get_filename_component(SOURCE_PATH_ABS "${SOURCE_FILE}" PATH)
		file(RELATIVE_PATH SOURCE_PATH "${CURRENT_DIRECTORY}" "${SOURCE_PATH_ABS}")

		# Transform string
		string(REPLACE "/" "\\" SOURCE_PATH "${SOURCE_PATH}")

		# Define group
		source_group("${BASE_DIRECTORY}\\${SOURCE_PATH}" FILES "${SOURCE_FILE}")
	ENDFOREACH()
endfunction()

# Include headers
include_directories(${LOG4CPP_INCLUDE_DIR})
include_directories(${AO_INCLUDE_DIR})

# Include sub-projects
add_subdirectory("src/main/vulkan")
add_subdirectory("src/main/core")
add_subdirectory("src/main/win-bootstrap")
