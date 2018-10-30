file(GLOB_RECURSE AO_CORE_SRC "*.cpp")
file(GLOB_RECURSE AO_CORE_HEADERS "*.h")

# Define library
add_library("ao-core" ${AO_CORE_SRC} ${AO_CORE_HEADERS})
target_link_libraries("ao-core" log4cpp)

# Copy headers in include directory
file(REMOVE_RECURSE "${AO_INCLUDE_DIR}/ao/core")
file(MAKE_DIRECTORY "${AO_INCLUDE_DIR}/ao/core")
file(COPY "." DESTINATION "${AO_INCLUDE_DIR}/ao/core" 
	FILES_MATCHING PATTERN "*.h"
	PATTERN "*.dir" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
)
