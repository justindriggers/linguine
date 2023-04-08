#pragma once

#include "Scene.h"

#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/GridPosition.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Selectable.h"
#include "components/Tile.h"
#include "components/Transform.h"
#include "data/Grid.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PointAndClickMovementSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class PointAndClickPrototypeScene : public Scene {
  public:
    explicit PointAndClickPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PointAndClickMovementSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.64f, 0.35f, 0.95f);

      {
        auto cameraEntity = createEntity();
        cameraEntity->add<Transform>();
        cameraEntity->add<PhysicalState>();

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->height = 20.0f;

        auto cameraCollider = cameraEntity->add<CircleCollider>();
        cameraCollider->radius = fixture->height;
      }

      for (auto x = 0; x < _grid->getWidth(); ++x) {
        for (auto y = 0; y < _grid->getHeight(); ++y) {
          auto entity = createEntity();
          entity->add<Tile>();

          auto transform = entity->add<Transform>();
          transform->position = glm::vec3(_grid->getWorldPosition({x, y}), 5.0f);
          transform->scale = glm::vec3(0.9f);

          auto drawable = entity->add<Drawable>();
          drawable->feature = new ColoredFeature();
          drawable->feature->meshType = Quad;
          drawable->feature->color = glm::vec3(0.06f, 0.06f, 0.06f);
          drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
          drawable.setRemovalListener([drawable](const Entity e) {
            drawable->renderable->destroy();
          });

          auto selectable = entity->add<Selectable>();
          selectable->feature = new SelectableFeature();
          selectable->feature->entityId = entity->getId();
          selectable->feature->meshType = Quad;
          selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
          selectable.setRemovalListener([selectable](const Entity e) {
            selectable->renderable->destroy();
          });
        }
      }

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();

        auto gridPosition = glm::vec2(2, 0);

        auto transform = playerEntity->add<Transform>();
        transform->position = glm::vec3(_grid->getWorldPosition(gridPosition), 1.0f);

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto gridPositionComponent = playerEntity->add<GridPosition>();
        gridPositionComponent->position = gridPosition;

        _grid->addObstruction(gridPositionComponent->position, gridPositionComponent->dimensions);

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
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(7, 8, 1.25f);
};

}  // namespace linguine
