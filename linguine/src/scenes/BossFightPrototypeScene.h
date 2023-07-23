#pragma once

#include "Scene.h"

#include "components/Alive.h"
#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/HealthBar.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/ProjectileAttack.h"
#include "components/Selectable.h"
#include "components/Targeting.h"
#include "components/Transform.h"
#include "components/Unit.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/DirectionalMovementSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemyTargetingSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class BossFightPrototypeScene : public Scene {
  public:
    explicit BossFightPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<DirectionalMovementSystem>(getEntityManager(), serviceLocator.get<InputManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<EnemyTargetingSystem>(getEntityManager()));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      {
        auto cameraEntity = createEntity();

        auto transform = cameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto physicalState = cameraEntity->add<PhysicalState>();
        physicalState->currentPosition = transform->position;

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->height = 20.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = glm::vec3(0.04f, 0.05f, 0.37f);
      }

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->height = 1.0f;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = {};
        fixture->camera->layer = UI;
      }

      {
        auto uiBackgroundEntity = createEntity();

        auto transform = uiBackgroundEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -0.35f, 10.0f);
        transform->scale = glm::vec3(1.0f, 0.3f, 0.0f);

        auto drawable = uiBackgroundEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = glm::vec3(0.15f);
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        auto selectable = uiBackgroundEntity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->meshType = Quad;
        selectable->feature->entityId = uiBackgroundEntity->getId();
        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });
      }

      {
        auto playerEntity = createEntity();
        playerEntity->add<Player>();
        playerEntity->add<Friendly>();
        playerEntity->add<Unit>();
        playerEntity->add<Alive>();

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(0.75f);
        transform->position = { 0.0f, 0.0f, 1.0f };

        auto physicalState = playerEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto circleCollider = playerEntity->add<CircleCollider>();
        circleCollider->radius = 0.75f / 2.0f;

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        const auto count = 5;

        for (int i = 0; i < count; ++i) {
          auto orbiterEntity = createEntity();
          orbiterEntity->add<Friendly>();

          orbiterEntity->add<Health>(500);
          orbiterEntity->add<Alive>();

          {
            auto healthEntity = createEntity();

            auto healthBar = healthEntity->add<HealthBar>();
            healthBar->entityId = orbiterEntity->getId();

            auto healthTransform = healthEntity->add<Transform>();
            healthTransform->position = glm::vec3((-static_cast<float>(count) / 2.0f + static_cast<float>(i) + 0.5f) * 0.08f, -0.3f, 0.0f);
            healthTransform->scale = glm::vec3(0.075f, 0.075f, 0.0f);

            auto healthProgressable = healthEntity->add<Progressable>();
            healthProgressable->feature = new ProgressFeature();
            healthProgressable->feature->meshType = Quad;
            healthProgressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(healthProgressable->feature), UI);
            healthProgressable.setRemovalListener([healthProgressable](const Entity e) {
              healthProgressable->renderable->destroy();
            });

            auto healthSelectable = healthEntity->add<Selectable>();
            healthSelectable->feature = new SelectableFeature();
            healthSelectable->feature->meshType = Quad;
            healthSelectable->feature->entityId = healthEntity->getId();
            healthSelectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(healthSelectable->feature), UI);
            healthSelectable.setRemovalListener([healthSelectable](const Entity e) {
              healthSelectable->renderable->destroy();
            });
          }
        }
      }

      {
        auto gcdEntity = createEntity();

        auto transform = gcdEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, -0.225f, 0.0f);
        transform->scale = glm::vec3(0.25f, 0.025f, 1.0f);

        auto progressable = gcdEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.88f, 0.34f, 0.1f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        auto globalCooldown = gcdEntity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      {
        auto enemyEntity = createEntity();
        enemyEntity->add<Hostile>();
        enemyEntity->add<Unit>();
        enemyEntity->add<Alive>();

        auto transform = enemyEntity->add<Transform>();
        transform->scale = glm::vec3(1.25f);
        transform->position = { 0.0f, 5.0f, 1.0f };

        auto physicalState = enemyEntity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto circleCollider = enemyEntity->add<CircleCollider>();
        circleCollider->radius = 1.25f / 2.0f;

        auto drawable = enemyEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->feature->color = { 1.0f, 0.0f, 0.0f };
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        auto targeting = enemyEntity->add<Targeting>();
        targeting->strategy = Targeting::Nearest;
        targeting->range = 50.0f;

        auto projectileAttack = enemyEntity->add<ProjectileAttack>();
        projectileAttack->speed = 7.5f;
        projectileAttack->frequency = 3.0f;
        projectileAttack->range = 50.0f;

        auto meleeAttack = enemyEntity->add<MeleeAttack>();
        meleeAttack->power = 500;
      }
    }
};

}  // namespace linguine
