#pragma once

#include "Scene.h"

#include <random>
#include <utility>

#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GridPosition.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Transform.h"
#include "data/Grid.h"
#include "data/rooms/RoomA.h"
#include "data/rooms/RoomB.h"
#include "data/rooms/RoomC.h"
#include "data/rooms/RoomLayout.h"
#include "data/rooms/SpawnRoom.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/PointAndClickMovementSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class ProceduralPrototypeScene : public Scene {
  public:
    explicit ProceduralPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PointAndClickMovementSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>(), *_grid));
      registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.64f, 0.65f, 0.11f);

      {
        auto cameraEntity = createEntity();

        auto transform = cameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto physicalState = cameraEntity->add<PhysicalState>();
        physicalState->currentPosition = transform->position;

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->height = 20.0f;
      }

      auto roomLayout = RoomLayout();

      std::unique_ptr<Room> previousRoom = std::make_unique<SpawnRoom>();

      auto origin = glm::ivec2(250, 250);
      previousRoom->layout(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);

      auto playerStartPosition = origin + glm::ivec2(previousRoom->getWidth(), previousRoom->getHeight()) / 2;

      auto rand = std::random_device();
      auto randomRoom = std::uniform_int_distribution(0, 2);
      auto randomDirection = std::uniform_int_distribution(0, 3);

      auto roomCount = 0;

      do {
          std::unique_ptr<Room> currentRoom;

          switch (randomRoom(rand)) {
            case 0:
              currentRoom = std::make_unique<RoomA>();
              break;
            case 1:
              currentRoom = std::make_unique<RoomB>();
              break;
            case 2:
              currentRoom = std::make_unique<RoomC>();
              break;
            default:
              throw std::runtime_error("Unexpected RNG value");
          }

          auto newOrigin = origin;

          switch (randomDirection(rand)) {
            case 0:
              newOrigin += previousRoom->getNorthDoor();
              newOrigin -= currentRoom->getSouthDoor();
              newOrigin += glm::ivec2(0, 1);
              break;
            case 1:
              newOrigin += previousRoom->getSouthDoor();
              newOrigin -= currentRoom->getNorthDoor();
              newOrigin -= glm::ivec2(0, 1);
              break;
            case 2:
              newOrigin += previousRoom->getEastDoor();
              newOrigin -= currentRoom->getWestDoor();
              newOrigin += glm::ivec2(1, 0);
              break;
            case 3:
              newOrigin += previousRoom->getWestDoor();
              newOrigin -= currentRoom->getEastDoor();
              newOrigin -= glm::ivec2(1, 0);
              break;
            default:
              throw std::runtime_error("Unexpected RNG value");
          }

          if (currentRoom->layout(getEntityManager(), serviceLocator, *_grid, roomLayout, newOrigin)) {
            previousRoom->enclose(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);
            previousRoom = std::move(currentRoom);
            origin = newOrigin;
            ++roomCount;
          }
      } while (roomCount < 16);

      previousRoom->enclose(getEntityManager(), serviceLocator, *_grid, roomLayout, origin);

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();
        playerEntity->add<Unit>();
        playerEntity->add<Alive>();

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(0.75f);
        transform->position = glm::vec3(_grid->getWorldPosition(playerStartPosition), 1.0f);

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto circleCollider = playerEntity->add<CircleCollider>();
        circleCollider->radius = 0.375f;

        auto gridPositionComponent = playerEntity->add<GridPosition>();
        gridPositionComponent->position = playerStartPosition;
        gridPositionComponent->speed = 2.0f;

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }
    }

  private:
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(500, 500, 1.0f);
};

}  // namespace linguine
