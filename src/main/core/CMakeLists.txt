file(GLOB_RECURSE AO_CORE_SRC "*.cpp")
file(GLOB_RECURSE AO_CORE_HEADERS "*.h" "*.hpp")

# Define library
create_library("ao-core" ${CMAKE_CURRENT_SOURCE_DIR} "fmt;${Boost_LOG_LIBRARY}")

# Copy headers in include directory
set(AO_CORE_INCLUDE_OUTPUT "${AO_INCLUDE_DIR}/ao/core")
file(REMOVE_RECURSE "${AO_CORE_INCLUDE_OUTPUT}")
file(MAKE_DIRECTORY "${AO_CORE_INCLUDE_OUTPUT}")
file(COPY "." DESTINATION "${AO_CORE_INCLUDE_OUTPUT}" 
	FILES_MATCHING REGEX ".*\\.h.*"
	PATTERN "*.dir" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
)

# Re-create sub-directories
IF (WIN32)
	create_sub_directories(AO_CORE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} "Header Files")
	create_sub_directories(AO_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR} "Source Files")
ENDIF()
