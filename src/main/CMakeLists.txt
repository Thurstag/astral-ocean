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

# Define a function that creates static library
function(create_library LIB_NAME LIB_DIRECTORY LIB_DEPENDENCIES)
	file(GLOB_RECURSE LIB_SOURCES ${LIB_DIRECTORY} "*.cpp" "*.h")

	# Static lib
	add_library("${LIB_NAME}" ${LIB_SOURCES} ${LIB_HEADERS})
	target_link_libraries("${LIB_NAME}" ${LIB_DEPENDENCIES})

	# Define group
	set_target_properties("${LIB_NAME}" PROPERTIES FOLDER "AO-LIBRARIES")
endfunction()

# Include projects
add_subdirectory("core")
add_subdirectory("vulkan")
IF(${AO_BUILD_BOOTSTRAPS})
	add_subdirectory("win-bootstrap")
ENDIF()
