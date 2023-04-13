#pragma once

#include "Scene.h"

#include "components/Alive.h"
#include "components/BigHeal.h"
#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Cooldown.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/HealthBar.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/Orbiter.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Tile.h"
#include "components/Transform.h"
#include "components/Unit.h"
#include "data/Grid.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/GridPositionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/OrbitSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerAttackSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/PlayerTargetingSystem.h"
#include "systems/PointAndClickMovementSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class PointAndClickPrototypeScene : public Scene {
  public:
    explicit PointAndClickPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PointAndClickMovementSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<OrbitSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<GridPositionSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PlayerTargetingSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<PlayerAttackSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager(), *_grid));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
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
        playerEntity->add<Friendly>();
        playerEntity->add<Targeting>();
        playerEntity->add<MeleeAttack>();

        auto health = playerEntity->add<Health>();
        health->max = 150;
        health->current = health->max;

        playerEntity->add<Alive>();

        auto gridPosition = glm::vec2(2, 0);

        auto transform = playerEntity->add<Transform>();
        transform->position = glm::vec3(_grid->getWorldPosition(gridPosition), 1.0f);

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto gridPositionComponent = playerEntity->add<GridPosition>();
        gridPositionComponent->position = gridPosition;
        gridPositionComponent->speed = 2.0f;

        _grid->addObstruction(gridPositionComponent->position, gridPositionComponent->dimensions);

        auto progressable = playerEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Triangle;
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        const auto count = 5;

        for (int i = 0; i < count; ++i) {
          auto orbiterEntity = createEntity();
          orbiterEntity->add<Friendly>();

          auto orbiter = orbiterEntity->add<Orbiter>();
          orbiter->centerId = playerEntity->getId();

          auto orbiterHealth = orbiterEntity->add<Health>();
          orbiterHealth->max = 500;
          orbiterHealth->current = orbiterHealth->max;

          orbiterEntity->add<Alive>();

          auto orbiterTransform = orbiterEntity->add<Transform>();
          orbiterTransform->scale = glm::vec3(0.25f);

          auto orbiterDrawable = orbiterEntity->add<Drawable>();
          orbiterDrawable->feature = new ColoredFeature();
          orbiterDrawable->feature->meshType = Quad;
          orbiterDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(orbiterDrawable->feature));
          orbiterDrawable.setRemovalListener([orbiterDrawable](const Entity e) {
            orbiterDrawable->renderable->destroy();
          });

          {
            auto healthEntity = createEntity();

            auto healthBar = healthEntity->add<HealthBar>();
            healthBar->entityId = orbiterEntity->getId();

            auto healthTransform = healthEntity->add<Transform>();
            healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 1.1f, -6.5f, 0.0f);

            auto healthProgressable = healthEntity->add<Progressable>();
            healthProgressable->feature = new ProgressFeature();
            healthProgressable->feature->meshType = Quad;
            healthProgressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(healthProgressable->feature));
            healthProgressable.setRemovalListener([healthProgressable](const Entity e) {
              healthProgressable->renderable->destroy();
            });

            auto healthSelectable = healthEntity->add<Selectable>();
            healthSelectable->feature = new SelectableFeature();
            healthSelectable->feature->meshType = Quad;
            healthSelectable->feature->entityId = healthEntity->getId();
            healthSelectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(healthSelectable->feature));
            healthSelectable.setRemovalListener([healthSelectable](const Entity e) {
              healthSelectable->renderable->destroy();
            });
          }
        }
      }

      {
        auto enemyEntity = createEntity();
        enemyEntity->add<Hostile>();
        enemyEntity->add<Unit>();
        enemyEntity->add<MeleeAttack>();

        auto health = enemyEntity->add<Health>();
        health->max = 5'000;
        health->current = health->max;

        enemyEntity->add<Alive>();
        enemyEntity->add<Targeting>();

        auto gridPosition = glm::vec2(3, 6);

        auto transform = enemyEntity->add<Transform>();
        transform->position = glm::vec3(_grid->getWorldPosition(gridPosition), 1.0f);

        auto physicalState = enemyEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto gridPositionComponent = enemyEntity->add<GridPosition>();
        gridPositionComponent->position = gridPosition;
        gridPositionComponent->speed = 1.5f;

        _grid->addObstruction(gridPositionComponent->position, gridPositionComponent->dimensions);

        auto progressable = enemyEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Triangle;
        progressable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
      }

      {
        auto gcdEntity = createEntity();

        auto transform = gcdEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -5.5f, 0.0f);
        transform->scale = glm::vec3(5.0f, 0.5f, 1.0f);

        auto progressable = gcdEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.88f, 0.34f, 0.1f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        auto globalCooldown = gcdEntity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      {
        auto bigHealEntity = createEntity();

        auto transform = bigHealEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -8.0f, 0.0f);
        transform->scale = glm::vec3(0.75f);

        auto progressable = bigHealEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.0f, 1.0f, 0.0f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto selectable = bigHealEntity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->entityId = bigHealEntity->getId();
        selectable->feature->meshType = Quad;
        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });

        bigHealEntity->add<BigHeal>();

        auto cooldown = bigHealEntity->add<Cooldown>();
        cooldown->elapsed = 10.0f;
        cooldown->total = 10.0f;
      }
    }

  private:
    std::unique_ptr<Grid> _grid = std::make_unique<Grid>(7, 8, 1.0f);
};

}  // namespace linguine
