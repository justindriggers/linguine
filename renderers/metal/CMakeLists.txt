add_library(metal
        src/MetalRenderBackend.cpp
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
        src/mesh/ShipMesh.cpp
        src/mesh/SpeedBoostMesh.cpp
        src/mesh/TimeWarpMesh.cpp
        src/mesh/TriangleMesh.cpp
        src/mesh/WingMesh.cpp
)

target_include_directories(metal PUBLIC include)
target_include_directories(metal PRIVATE src)

target_link_libraries(metal PRIVATE linguine)

find_library(METAL_LIBRARY Metal REQUIRED)
target_link_libraries(metal PRIVATE ${METAL_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(metal PRIVATE ${METALKIT_LIBRARY})

target_include_directories(metal PUBLIC
        "${THIRD_PARTY_DIR}/metal-cpp"
        "${THIRD_PARTY_DIR}/metal-cpp-extensions"
)
