project(pesto)

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(pesto
        src/main.cpp
        src/platform/WebInputManager.cpp
        src/platform/WebLogger.cpp
        src/platform/WebOpenALFileLoader.cpp
        src/platform/WebTimeManager.cpp
)

target_link_libraries(pesto PRIVATE linguine openal opengl)

set_target_properties(pesto PROPERTIES LINK_FLAGS "-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")

add_custom_target(pesto-run
        COMMAND emrun --browser chrome pesto.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pesto
)
add_dependencies(pesto-run pesto)
