SET(MAIN_CPP main.cpp)

# Include *.cpp & vk *.h
file(GLOB SOURCE *.cpp ${VK_HEADERS})

# Define executable
add_executable(win-bootstrap ${MAIN_CPP} ${SOURCE})

# Import libraries
target_link_libraries(win-bootstrap vulkan core)
install(TARGETS win-bootstrap DESTINATION "${CMAKE_INSTALL_BINDIR}/bin")
