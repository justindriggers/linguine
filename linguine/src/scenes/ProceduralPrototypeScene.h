#pragma once

#include "Scene.h"

#include <random>

#include "components/CameraFixture.h"
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
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/PointAndClickMovementSystem.h"
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
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
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
        fixture->height = 10.0f;
      }

      auto rand = std::random_device();

      auto randomRoom = std::uniform_int_distribution(0, 2);

      std::unique_ptr<Room> room1;

      switch (randomRoom(rand)) {
        case 0:
          room1 = std::make_unique<RoomA>();
          break;
        case 1:
          room1 = std::make_unique<RoomB>();
          break;
        case 2:
          room1 = std::make_unique<RoomC>();
          break;
        default:
          throw std::runtime_error("Unexpected RNG value");
      }

      std::unique_ptr<Room> room2;

      switch (randomRoom(rand)) {
        case 0:
          room2 = std::make_unique<RoomA>();
          break;
        case 1:
          room2 = std::make_unique<RoomB>();
          break;
        case 2:
          room2 = std::make_unique<RoomC>();
          break;
        default:
          throw std::runtime_error("Unexpected RNG value");
      }

      auto randomDirection = std::uniform_int_distribution(0, 3);

      auto origin = glm::ivec2(25, 25);
      room1->layout(getEntityManager(), serviceLocator, *_grid, origin);

      auto direction = randomDirection(rand);

      switch (direction) {
        case 0:
          origin += room1->getNorthDoor();
          origin -= room2->getSouthDoor();
          break;
        case 1:
          origin += room1->getSouthDoor();
          origin -= room2->getNorthDoor();
          break;
        case 2:
          origin += room1->getEastDoor();
          origin -= room2->getWestDoor();
          break;
        case 3:
          origin += room1->getWestDoor();
          origin -= room2->getEastDoor();
          break;
        default:
          throw std::runtime_error("Unexpected RNG value");
      }

      room2->layout(getEntityManager(), serviceLocator, *_grid, origin);

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();

        auto gridPosition = origin + glm::ivec2(room1->getHeight(), room2->getHeight()) / 2;

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(0.75f);
        transform->position = glm::vec3(_grid->getWorldPosition(gridPosition), 1.0f);

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto gridPositionComponent = playerEntity->add<GridPosition>();
        gridPositionComponent->position = gridPosition;
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
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(50, 50, 1.0f);
};

}  // namespace linguine
