project(pesto)

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(pesto
        src/main.cpp
        src/platform/WebInputManager.cpp
        src/platform/WebLogger.cpp
        src/platform/WebOpenALFileLoader.cpp
        src/platform/WebOpenGLFileLoader.cpp
        src/platform/WebTimeManager.cpp
        src/platform/WebUrlHandler.cpp
)

target_include_directories(pesto PUBLIC
        "${THIRD_PARTY_DIR}/stb"
)

find_package(OpenGL REQUIRED)
target_link_libraries(pesto PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(pesto PRIVATE ${OPENGL_INCLUDE_DIR})

find_package(OpenAL REQUIRED)
target_link_libraries(pesto PRIVATE ${OPENAL_LIBRARIES})
target_include_directories(pesto PRIVATE ${OPENAL_INCLUDE_DIR})

target_link_libraries(pesto PRIVATE linguine openal opengl)

set_target_properties(pesto PROPERTIES LINK_FLAGS "-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")

add_custom_target(pesto-run
        COMMAND emrun --browser chrome pesto.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pesto
)
add_dependencies(pesto-run pesto)
