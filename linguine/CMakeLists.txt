add_library(linguine
        src/Engine.cpp
        src/entity/archetype/Archetype.cpp
        src/entity/archetype/ArchetypeEntity.cpp
        src/entity/archetype/ArchetypeEntityManager.cpp
        src/entity/archetype/ArchetypeResult.cpp
        src/entity/archetype/Query.cpp
        src/entity/archetype/Store.cpp
        src/renderer/Renderable.cpp
        src/renderer/Renderer.cpp
        src/renderer/features/FeatureRenderer.cpp
)

target_include_directories(linguine PUBLIC
        include
        "${THIRD_PARTY_DIR}/glm"
)
