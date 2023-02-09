add_library(metal
        src/MetalRenderer.cpp
        src/features/QuadFeatureRenderer.cpp
        src/features/TriangleFeatureRenderer.cpp
)

target_include_directories(metal PUBLIC include)

target_link_libraries(metal PRIVATE linguine)

find_library(METAL_LIBRARY Metal REQUIRED)
target_link_libraries(metal PRIVATE ${METAL_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(metal PRIVATE ${METALKIT_LIBRARY})

target_include_directories(metal PUBLIC
        "${THIRD_PARTY_DIR}/metal-cpp"
        "${THIRD_PARTY_DIR}/metal-cpp-extensions"
)
