add_library(opengl
        src/OpenGLRenderer.cpp
        src/features/ColoredFeatureRenderer.cpp
        src/features/ProgressFeatureRenderer.cpp
        src/features/SelectableFeatureRenderer.cpp
        src/features/TextFeatureRenderer.cpp
        src/mesh/MeshRegistry.cpp
        src/mesh/QuadMesh.cpp
        src/mesh/ScreenMesh.cpp
        src/mesh/TriangleMesh.cpp
        src/postprocessing/GammaCorrection.cpp
)

target_include_directories(opengl PUBLIC include)
target_include_directories(opengl PRIVATE src)

target_link_libraries(opengl PRIVATE linguine)

find_package(OpenGL REQUIRED)
target_link_libraries(opengl PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(opengl PRIVATE ${OPENGL_INCLUDE_DIR})
