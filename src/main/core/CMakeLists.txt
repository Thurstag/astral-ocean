file(GLOB CORE_SRC "*.cpp")
file(GLOB CORE_HEADERS "*.h")

# Define library
add_library(core STATIC ${CORE_HEADERS} ${CORE_SRC})
target_link_libraries(core log4cpp)
