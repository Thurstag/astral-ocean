SET(MAIN_CPP main.cpp)

# Include *.cpp & vk *.h
file(GLOB_RECURSE SOURCE *.cpp *.h)

# Define executable
add_executable("win-bootstrap" ${MAIN_CPP} ${SOURCE})
set_target_properties("win-bootstrap" PROPERTIES  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bootstraps")

# Import libraries
target_link_libraries("win-bootstrap" "ao-vulkan")
#install(TARGETS "win-bootstrap")
