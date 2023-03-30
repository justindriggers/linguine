#pragma once

#include "Scene.h"

#include "components/Alive.h"
#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Event.h"
#include "components/ExplosionAttack.h"
#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/PhysicalState.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/SpawnUnit.h"
#include "components/Targeting.h"
#include "components/Tile.h"
#include "components/Transform.h"
#include "components/Unit.h"
#include "data/Grid.h"
#include "systems/AttackSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/EventSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/TileSelectionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class TowerPrototypeScene : public Scene {
  public:
    explicit TowerPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      _projectileFactory = std::make_unique<ProjectileFactory>(getEntityManager(), serviceLocator.get<Renderer>());

      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<EventSystem>(getEntityManager()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<TileSelectionSystem>(getEntityManager(), serviceLocator.get<Renderer>(), *_grid));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<AttackSystem>(getEntityManager(), *_projectileFactory));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.34f, 0.55f, 0.04f);

      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();
      cameraEntity->add<Transform>();

      for (auto x = 0; x < _width; ++x) {
        for (auto y = 0; y < _height; ++y) {
          auto entity = createEntity();
          entity->add<Tile>();

          if (x == 1 && y == 2) {
            entity->add<SpawnUnit>();
          }

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

      for (auto i = 0; i < 5; ++i) {
        auto eventEntity = createEntity();

        auto event = eventEntity->add<Event>();
        event.setRemovalListener([](Entity entity) {
          entity.destroy();
        });
        event->secondsRemaining = static_cast<float>(i + 1) * 5.0f;
        event->function = [this, &renderer]() {
          auto enemy = createEntity();
          enemy->add<Hostile>();

          enemy->add<Unit>();

          auto meleeAttack = enemy->add<MeleeAttack>();
          meleeAttack->speed = 1.5f;

          auto explosionAttack = enemy->add<ExplosionAttack>();
          explosionAttack->frequency = 3.0f;
          explosionAttack->speed = 5.0f;

          auto transform = enemy->add<Transform>();
          transform->position = glm::vec3(_grid->getWorldPosition({2, 5}), 1.0f);

          auto physicalState = enemy->add<PhysicalState>();
          physicalState->previousPosition = glm::vec2(transform->position);
          physicalState->currentPosition = physicalState->previousPosition;

          auto gridPosition = enemy->add<GridPosition>();
          gridPosition->position = {2, 5};

          enemy->add<CircleCollider>();

          auto progressable = enemy->add<Progressable>();
          progressable->feature = new ProgressFeature();
          progressable->feature->meshType = Triangle;
          progressable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
          progressable->renderable = renderer.create(
              std::unique_ptr<ProgressFeature>(progressable->feature));
          progressable.setRemovalListener([progressable](const Entity e) {
            progressable->renderable->destroy();
          });

          auto health = enemy->add<Health>();
          health->current = 10'000;
          health->max = 10'000;

          enemy->add<Alive>();

          auto targeting = enemy->add<Targeting>();
          targeting->strategy = Targeting::Nearest;
        };
      }
    }

  private:
    constexpr static int _width = 5;
    constexpr static int _height = 6;
    constexpr static float _scale = 1.25f;

    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(_width, _height, _scale);

    std::unique_ptr<ProjectileFactory> _projectileFactory;
};

}  // namespace linguine
