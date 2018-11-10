cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Define build variables
option(AO_BUILD_TESTS "Build tests" ON)
option(AO_BUILD_BOOTSTRAPS "Build bootstraps" ON)

# Define variables
set(PROJECT_NAME "astral-ocean")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(SUBMODULES_DIR "${CMAKE_BINARY_DIR}/submodules")
set(glfw_INCLUDE_DIR "${SUBMODULES_DIR}/glfw/include")
set(log4cpp_INCLUDE_DIR "${SUBMODULES_DIR}/log4cpp/include")
set(googletest_INCLUDE_DIR "${SUBMODULES_DIR}/googletest/include" "${SUBMODULES_DIR}/googlemock/include")
set(AO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Define dependencies
set(DEPENDENCIES Vulkan Threads Boost GLM)
set(SUBMODULES glfw log4cpp)

IF(AO_BUILD_TESTS)
	set(SUBMODULES ${SUBMODULES} googletest)
ENDIF()

# Disable GLFW unwanted builds
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Prevent overriding the parent project's compiler/linker
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

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
		find_package(${DEPENDENCY} REQUIRED)
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
	include_directories(${${SUBMODULE}_INCLUDE_DIR})
ENDFOREACH(SUBMODULE)
include_directories("${SUBMODULES_DIR}/CTPL/")

# Include headers
include_directories(${AO_INCLUDE_DIR})

# Include sub-projects
add_subdirectory("src/main")
IF(${AO_BUILD_TESTS})
	enable_testing()
	add_subdirectory("src/test")
ENDIF()

# Force folder creation
set_target_properties(log4cpp PROPERTIES FOLDER "LOG4C++")
IF(AO_BUILD_BOOTSTRAPS AND WIN32)
	set_target_properties("win-bootstrap" PROPERTIES FOLDER "AO-BOOTSTRAPS")
ENDIF()
IF(AO_BUILD_TESTS)
	set_target_properties(gmock gmock_main gtest gtest_main PROPERTIES FOLDER "GOOGLE-TEST")
ENDIF()
