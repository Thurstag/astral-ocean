SET(MAIN_CPP main.cpp)

# Include *.cpp & vk *.h
file(GLOB_RECURSE SOURCE *.cpp)

# Define executable
add_executable(win-bootstrap ${MAIN_CPP} ${SOURCE})

# Import libraries
target_link_libraries(win-bootstrap "ao-vulkan" "ao-core")
install(TARGETS win-bootstrap DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
