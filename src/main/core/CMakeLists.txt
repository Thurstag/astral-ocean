file(GLOB_RECURSE AO_CORE_SRC "*.cpp")
file(GLOB_RECURSE AO_CORE_HEADERS "*.h" "*.hpp")

# Define library
create_library("ao-core" ${CMAKE_CURRENT_SOURCE_DIR} "fmt;${Boost_LOG_LIBRARY};${Boost_LOG_SETUP_LIBRARY};${Boost_THREAD_LIBRARY};${Boost_SYSTEM_LIBRARY}")

# Re-create sub-directories
create_sub_directories(AO_CORE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} "Header Files")
create_sub_directories(AO_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR} "Source Files")
