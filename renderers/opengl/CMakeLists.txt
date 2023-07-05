add_library(opengl
        src/OpenGLRenderer.cpp
)

target_include_directories(opengl PUBLIC include)
target_include_directories(opengl PRIVATE src)

target_link_libraries(opengl PRIVATE linguine)

find_package(OpenGL REQUIRED)
target_link_libraries(opengl PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(opengl PRIVATE ${OPENGL_INCLUDE_DIR})
