project(pesto)

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(pesto
        src/main.cpp
        src/platform/WebLogger.cpp
        src/platform/WebTimeManager.cpp
)

target_link_libraries(pesto PRIVATE linguine opengl)

add_custom_target(pesto-run
        COMMAND emrun --browser chrome pesto.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pesto
)
add_dependencies(pesto-run pesto)
