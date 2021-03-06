cmake_minimum_required(VERSION 3.12.3 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

########################################## PROJECT ##########################################

# Define build variables
option(AO_BUILD_TESTS "Build tests" ON)
option(AO_BUILD_STATIC_LIB "Build static" ON)

# Define project variables
set(AO_LIB_PROJECT_NAME "astral-ocean")
set(AO_LIB_PROJECT_VERSION "0.0.0")
set(AO_LIB_SUBMODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules")

# Define project name
project(${AO_LIB_PROJECT_NAME} VERSION ${AO_LIB_PROJECT_VERSION})

################################# DEPENDENCIES DECLARATION ##################################

# Define dependencies
set(DEPENDENCIES)
set(AO_LIB_SUBMODULES fmt vulkan volk boost-cmake)
if(AO_BUILD_TESTS)
	set(AO_LIB_SUBMODULES ${AO_LIB_SUBMODULES} googletest)
endif()

# Define includes
set(fmt_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}/fmt/include")
set(vulkan_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}")
set(volk_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}")
set(AO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(googletest_INCLUDE_DIR "${AO_LIB_SUBMODULES_DIR}/googletest/include;${AO_LIB_SUBMODULES_DIR}/googlemock/include")

# Disable FMT unwanted builds
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)

# Prevent overriding the parent project's compiler/linker
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

########################################## CMAKE ############################################

# Cmake settings
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

######################################### COMPILER ##########################################

# Force c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define compile definitions
if(APPLE)
	add_compile_definitions(_GNU_SOURCE BOOST_THREAD_USES_DATETIME)
elseif(UNIX)
	add_compile_definitions(BOOST_THREAD_USES_DATETIME)
else()
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		add_compile_definitions(BOOST_THREAD_USE_LIB)
	endif()
endif()

################################### INCLUDE DEPENDENCIES ####################################

# Try to find external libraries
foreach(DEPENDENCY ${DEPENDENCIES})
	find_package(${DEPENDENCY} REQUIRED)
endforeach(DEPENDENCY)

# Check all external libraries
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
foreach(SUBMODULE ${AO_LIB_SUBMODULES})
	message("Include ${SUBMODULE} submodule")

	if(EXISTS "${AO_LIB_SUBMODULES_DIR}/${SUBMODULE}/CMakeLists.txt")
		add_subdirectory("${AO_LIB_SUBMODULES_DIR}/${SUBMODULE}")
	endif()

	# Check if include is a list
	list(LENGTH ${SUBMODULE}_INCLUDE_DIR LIST_LENGTH)

	if(LIST_LENGTH GREATER 1)
		include_directories(${${SUBMODULE}_INCLUDE_DIR})
	elseif(EXISTS ${${SUBMODULE}_INCLUDE_DIR})
		include_directories(${${SUBMODULE}_INCLUDE_DIR})
	elseif(LIST_LENGTH GREATER 1)
		message(WARNING "Unable to include ${SUBMODULE} headers (path: ${${SUBMODULE}_INCLUDE_DIR})")
	endif()
endforeach(SUBMODULE)

############################################ AO #############################################

# Include ao headers
include_directories(${AO_INCLUDE_DIR})

# Include sub-projects
add_subdirectory("src/main")
if(${AO_BUILD_TESTS})
	enable_testing()
	add_subdirectory("src/tests")
endif()

# Force folder creation
if(TARGET fmt)
	set_target_properties(fmt PROPERTIES FOLDER "FMT")
endif()
if(TARGET volk)
	set_target_properties(volk PROPERTIES FOLDER "VOLK")
endif()
if((AO_BUILD_TESTS) AND ((TARGET gmock) AND (TARGET gmock_main) AND (TARGET gtest) AND (TARGET gtest_main)))
	set_target_properties(gmock gmock_main gtest gtest_main PROPERTIES FOLDER "GOOGLE-TEST")
endif()
