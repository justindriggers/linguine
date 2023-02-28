add_library(metal
        src/MetalRenderer.cpp
        src/features/ColoredFeatureRenderer.cpp
        src/features/SelectableFeatureRenderer.cpp
        src/mesh/MeshRegistry.cpp
        src/mesh/QuadMesh.cpp
        src/mesh/TriangleMesh.cpp
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
