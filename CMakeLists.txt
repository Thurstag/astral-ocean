cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

# Define build variables
option(AO_BUILD_TESTS "Build tests" ON)
option(AO_BUILD_STATIC_LIB "Build static" ON)

# Define variables
set(AO_LIB_PROJECT_NAME "astral-ocean")
set(AO_LIB_PROJECT_VERSION "0.0.0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(AO_LIB_SUBMODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules")
set(googletest_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}/googletest/include" "${AO_LIB_SUBMODULES_DIR}/googlemock/include")

# Define dependencies
set(DEPENDENCIES Vulkan Threads)
set(SUBMODULES fmt)

# Define includes
set(fmt_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}/fmt/include")
set(AO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(AO_BUILD_TESTS)
	set(SUBMODULES ${SUBMODULES} googletest)
endif()

# Disable FMT unwanted builds
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)

# Prevent overriding the parent project's compiler/linker
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Cmake settings
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Force c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define project name
project(${AO_LIB_PROJECT_NAME} VERSION ${AO_LIB_PROJECT_VERSION})

# Try to find libraries
if(NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	foreach(DEPENDENCY ${DEPENDENCIES})
		find_package(${DEPENDENCY} REQUIRED)
	endforeach(DEPENDENCY)
endif()

# Boost lib
find_package(Boost 1.68 COMPONENTS log log_setup thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# Check all libs
foreach(DEPENDENCY ${DEPENDENCIES})
	if(NOT ${${DEPENDENCY}_FOUND})
		message(FATAL_ERROR "Could not find ${DEPENDENCY} library!")
	else()
		set(LIB_PATH "${${DEPENDENCY}_INCLUDE_DIR}")
		if(LIB_PATH STREQUAL "")
			message("${DEPENDENCY} library found")
		else()
			message("${DEPENDENCY} library found: '${LIB_PATH}', auto-include it")
			include_directories(${LIB_PATH})
		endif()
	endif()
endforeach(DEPENDENCY)

# Add submodules
foreach(SUBMODULE ${SUBMODULES})
	message("Include ${SUBMODULE} submodule")
	
	add_subdirectory("${AO_LIB_SUBMODULES_DIR}/${SUBMODULE}")
	include_directories(${${SUBMODULE}_INCLUDE_DIR})
endforeach(SUBMODULE)
include_directories("${AO_LIB_SUBMODULES_DIR}/CTPL/")

# Include lib headers
include_directories(${AO_INCLUDE_DIR})

# Include sub-projects
add_subdirectory("src/main")
if(${AO_BUILD_TESTS})
	enable_testing()
	add_subdirectory("src/tests")
endif()

# Force folder creation
set_target_properties(fmt PROPERTIES FOLDER "FMT")
if(AO_BUILD_TESTS)
	set_target_properties(gmock gmock_main gtest gtest_main PROPERTIES FOLDER "GOOGLE-TEST")
endif()
