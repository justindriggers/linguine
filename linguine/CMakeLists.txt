add_library(linguine
        src/Engine.cpp
        src/renderer/Renderable.cpp
        src/renderer/Renderer.cpp
        src/renderer/features/FeatureRenderer.cpp
)

target_include_directories(linguine PUBLIC
        include
        "${THIRD_PARTY_DIR}/glm"
)
