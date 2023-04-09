add_library(linguine
        src/Engine.cpp
        src/data/Grid.cpp
        src/entity/archetype/Archetype.cpp
        src/entity/archetype/ArchetypeEntityManager.cpp
        src/entity/archetype/ArchetypeResult.cpp
        src/entity/archetype/Query.cpp
        src/entity/archetype/Store.cpp
        src/factories/ProjectileFactory.cpp
        src/renderer/Renderable.cpp
        src/renderer/Renderer.cpp
        src/renderer/features/FeatureRenderer.cpp
        src/systems/AttackSystem.cpp
        src/systems/CameraSystem.cpp
        src/systems/CollisionSystem.cpp
        src/systems/CooldownProgressSystem.cpp
        src/systems/EnemyTargetingSystem.cpp
        src/systems/EnemySpawnSystem.cpp
        src/systems/EventSystem.cpp
        src/systems/FallerSystem.cpp
        src/systems/FpsSystem.cpp
        src/systems/GestureRecognitionSystem.cpp
        src/systems/GridPositionSystem.cpp
        src/systems/HealthProgressSystem.cpp
        src/systems/LivenessSystem.cpp
        src/systems/OrbitSystem.cpp
        src/systems/PathTraversalSystem.cpp
        src/systems/PhysicsInterpolationSystem.cpp
        src/systems/PlayerControllerSystem.cpp
        src/systems/PointAndClickMovementSystem.cpp
        src/systems/ProjectileSpawnSystem.cpp
        src/systems/ProjectileSystem.cpp
        src/systems/RiserSystem.cpp
        src/systems/RotatorSystem.cpp
        src/systems/SelectionDestructionSystem.cpp
        src/systems/TileSelectionSystem.cpp
        src/systems/TransformationSystem.cpp
        src/systems/UnitCreationSystem.cpp
)

target_include_directories(linguine PUBLIC
        include
        "${THIRD_PARTY_DIR}/glm"
)

target_include_directories(linguine PRIVATE
        src
)
