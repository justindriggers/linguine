add_library(opengl
        src/OpenGLRenderer.cpp
        src/features/CircleFeatureRenderer.cpp
        src/features/ColoredFeatureRenderer.cpp
        src/features/ProgressFeatureRenderer.cpp
        src/features/SelectableFeatureRenderer.cpp
        src/features/TextFeatureRenderer.cpp
        src/mesh/BoosterMesh.cpp
        src/mesh/CockpitMesh.cpp
        src/mesh/MeshRegistry.cpp
        src/mesh/MineMesh.cpp
        src/mesh/PlusMesh.cpp
        src/mesh/QuadMesh.cpp
        src/mesh/ReviveMesh.cpp
        src/mesh/ScreenMesh.cpp
        src/mesh/ShipMesh.cpp
        src/mesh/SpeedBoostMesh.cpp
        src/mesh/TimeWarpMesh.cpp
        src/mesh/TriangleMesh.cpp
        src/mesh/WingMesh.cpp
        src/postprocessing/GammaCorrection.cpp
)

target_include_directories(opengl PUBLIC include)
target_include_directories(opengl PRIVATE src)

target_link_libraries(opengl PRIVATE linguine)
