# Define a function that can re-create sub-directories in visual studio
function(create_sub_directories FILE_LIST CURRENT_DIRECTORY BASE_DIRECTORY)
	foreach(SOURCE_FILE IN LISTS ${FILE_LIST})
		# Get relative path
		get_filename_component(SOURCE_PATH_ABS "${SOURCE_FILE}" PATH)
		file(RELATIVE_PATH SOURCE_PATH "${CURRENT_DIRECTORY}" "${SOURCE_PATH_ABS}")

		# Transform string
		string(REPLACE "/" "\\" SOURCE_PATH "${SOURCE_PATH}")

		# Define group
		source_group("${BASE_DIRECTORY}\\${SOURCE_PATH}" FILES "${SOURCE_FILE}")
	endforeach()
endfunction()

# Define a function that creates static library
function(create_library LIB_NAME LIB_DIRECTORY LIB_DEPENDENCIES)
	file(GLOB_RECURSE LIB_SOURCES ${LIB_DIRECTORY} "*.cpp" "*.h" "*.hpp")

	# Lib
	if(${AO_BUILD_STATIC_LIB})
		add_library("${LIB_NAME}" STATIC ${LIB_SOURCES} ${LIB_HEADERS})
	else()
		add_library("${LIB_NAME}" SHARED ${LIB_SOURCES} ${LIB_HEADERS})
	endif()
	target_link_libraries("${LIB_NAME}" ${LIB_DEPENDENCIES} ${CMAKE_DL_LIBS})

	# Define properties
	set_target_properties("${LIB_NAME}" PROPERTIES 
		FOLDER "AO-LIB" 
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
		VERSION ${AO_LIB_PROJECT_VERSION}
	)
endfunction()

# Include libraries
add_subdirectory("core")
add_subdirectory("vulkan")
