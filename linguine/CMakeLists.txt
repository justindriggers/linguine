add_library(linguine
        src/Engine.cpp
        src/entity/archetype/Archetype.cpp
        src/entity/archetype/ArchetypeEntityManager.cpp
        src/entity/archetype/ArchetypeResult.cpp
        src/entity/archetype/Query.cpp
        src/entity/archetype/Store.cpp
        src/renderer/Renderable.cpp
        src/renderer/Renderer.cpp
        src/renderer/features/FeatureRenderer.cpp
        src/systems/CameraSystem.cpp
        src/systems/EnemyAttackSystem.cpp
        src/systems/EnemySpawnSystem.cpp
        src/systems/FallerSystem.cpp
        src/systems/FpsSystem.cpp
        src/systems/FriendlyAttackSystem.cpp
        src/systems/GestureRecognitionSystem.cpp
        src/systems/HealthProgressSystem.cpp
        src/systems/LivenessSystem.cpp
        src/systems/PlayerControllerSystem.cpp
        src/systems/RiserSystem.cpp
        src/systems/RotatorSystem.cpp
        src/systems/SelectionDestructionSystem.cpp
        src/systems/TransformationSystem.cpp
)

target_include_directories(linguine PUBLIC
        include
        "${THIRD_PARTY_DIR}/glm"
)

target_include_directories(linguine PRIVATE
        src
)
