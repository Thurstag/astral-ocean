file(GLOB_RECURSE AO_TEST_FILES "*.cpp")
file(GLOB_RECURSE AO_HELPER_FILES "*.hpp" "*.h")

foreach(TEST_FILE IN LISTS AO_TEST_FILES)
	# Get relative path & file's name
	get_filename_component(TEST_FILE_ABS_PATH "${TEST_FILE}" PATH)
	file(RELATIVE_PATH TEST_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${TEST_FILE_ABS_PATH}")
	get_filename_component(TEST_FILE_NAME "${TEST_FILE}" NAME)

	# Remove extension
	string(REPLACE ".cpp" "" TEST_FILE_NAME "${TEST_FILE_NAME}")

	# Define executable
	add_executable("${TEST_FILE_NAME}" "${TEST_FILE}" "${AO_HELPER_FILES}")
	set_target_properties("${TEST_FILE_NAME}" PROPERTIES FOLDER "AO-TESTS/${TEST_FILE_PATH}" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests")

	# Link libraries
	target_link_libraries("${TEST_FILE_NAME}" "gtest_main;ao-vulkan")

	# Define test
	add_test(NAME "${TEST_FILE_NAME}" COMMAND "${TEST_FILE_NAME}" WORKING_DIRECTORY) 
endforeach()

# Re-create sub-directories
create_sub_directories(AO_HELPER_FILES ${CMAKE_CURRENT_SOURCE_DIR} "")
