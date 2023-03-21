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
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Path.h"
#include "components/PhysicalState.h"
#include "components/Progressable.h"
#include "components/Rotating.h"
#include "components/Selectable.h"
#include "components/SpawnPoint.h"
#include "components/Transform.h"
#include "components/Unit.h"
#include "data/CubicBezierCurve.h"
#include "data/Spline.h"
#include "factories/ProjectileFactory.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/FpsSystem.h"
#include "systems/FriendlyAttackSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/PathTraversalSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/ProjectileSpawnSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/RotatorSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class BulletPrototypeScene : public Scene {
  public:
    explicit BulletPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()),
          _renderer(serviceLocator.get<Renderer>()) {
      _projectileFactory = std::make_unique<ProjectileFactory>(getEntityManager(), serviceLocator.get<Renderer>());

      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<PathTraversalSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSpawnSystem>(getEntityManager(), *_projectileFactory));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<RotatorSystem>(getEntityManager(), serviceLocator.get<AudioManager>()));
      registerSystem(std::make_unique<FriendlyAttackSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      _renderer.getCamera().clearColor = glm::vec3(0.03f, 0.04f, 0.07f);

      {
        auto cameraEntity = createEntity();
        cameraEntity->add<Transform>();
        cameraEntity->add<PhysicalState>();

        auto fixture = cameraEntity->add<CameraFixture>();

        auto cameraCollider = cameraEntity->add<CircleCollider>();
        cameraCollider->radius = fixture->height;
      }

      {
        auto rotator = createEntity();
        rotator->add<Hostile>();
        rotator->add<Alive>();
        rotator->add<Unit>();
        rotator->add<Transform>();
        rotator->add<PhysicalState>();
        rotator->add<CircleCollider>();

        auto spawnPoint = rotator->add<SpawnPoint>();
        spawnPoint->count = 6;
        spawnPoint->interval = 1.0f / 3.0f;

        auto rotating = rotator->add<Rotating>();
        rotating->speed = 0.07f;

        auto progressable = rotator->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Triangle;
        progressable->feature->color = glm::vec3(1.0f, 1.0f, 1.0f);
        progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto health = rotator->add<Health>();
        health->current = 10'000;
        health->max = 10'000;
      }

      {
        auto ring = createEntity();
        ring->add<Transform>();
        ring->add<PhysicalState>();

        auto spawnPoint = ring->add<SpawnPoint>();
        spawnPoint->count = 72;
        spawnPoint->interval = 5.0f;
      }

      {
        // Global Cooldown Indicator
        auto entity = createEntity();

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(0.0f, -4.0f, 0.0f);

        auto progressable = entity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.88f, 0.34f, 0.1f);
        progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        auto globalCooldown = entity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      auto c0 = CubicBezierCurve{
          {
              { 0.0f, 1.0f },
              { 0.552284749831f, 1.0f },
              { 1.0f, 0.552284749831f },
              { 1.0f, 0.0f }
          }
      };

      auto c1 = CubicBezierCurve{
          {
              { 1.0f, 0.0f },
              { 1.0f, -0.552284749831f },
              { 0.552284749831f, -1.0f },
              { 0.0f, -1.0f }
          }
      };

      auto c2 = CubicBezierCurve{
          {
              { 0.0f, -1.0f },
              { -0.552284749831f, -1.0f },
              { -1.0f, -0.552284749831f },
              { -1.0f, 0.0f }
          }
      };

      auto c3 = CubicBezierCurve{
          {
              { -1.0f, 0.0f },
              { -1.0f, 0.552284749831f },
              { -0.552284749831f, 1.0f },
              { 0.0f, 1.0f }
          }
      };

      auto spline = Spline{
          { c0, c1, c2, c3 }
      };

      createFriendly(spline, 0);
      createFriendly(spline, 1);
      createFriendly(spline, 2);
      createFriendly(spline, 3);

      {
        // Big Heal
        auto entity = createEntity();

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(0.0f, -6.0f, 0.0f);
        transform->scale = glm::vec3(1.25f);

        auto progressable = entity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.0f, 1.0f, 0.0f);
        progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto selectable = entity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->entityId = entity->getId();
        selectable->feature->meshType = Quad;
        selectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
        selectable.setRemovalListener([selectable](const Entity e) {
          selectable->renderable->destroy();
        });

        entity->add<BigHeal>();

        auto cooldown = entity->add<Cooldown>();
        cooldown->elapsed = 10.0f;
        cooldown->total = 10.0f;
      }
    }

  private:
    Renderer& _renderer;
    std::unique_ptr<ProjectileFactory> _projectileFactory;

    void createFriendly(Spline& spline, int startIndex) {
      auto entity = createEntity();
      entity->add<Friendly>();

      auto unit = entity->add<Unit>();
      unit->attackSpeed = 0.85f;

      entity->add<Transform>();
      entity->add<PhysicalState>();
      entity->add<CircleCollider>();

      auto progressable = entity->add<Progressable>();
      progressable->feature = new ProgressFeature();
      progressable->feature->meshType = Quad;
      progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
      progressable.setRemovalListener([progressable](const Entity e) {
        progressable->renderable->destroy();
      });

      auto selectable = entity->add<Selectable>();
      selectable->feature = new SelectableFeature();
      selectable->feature->entityId = entity->getId();
      selectable->feature->meshType = Quad;
      selectable->renderable = _renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
      selectable.setRemovalListener([selectable](const Entity e) {
        selectable->renderable->destroy();
      });

      auto health = entity->add<Health>();
      health->current = 1'000;
      health->max = 1'000;

      entity->add<Alive>();

      auto path = entity->add<Path>();
      path->spline = spline;
      path->scale = 2.5f;
      path->speed = 0.5f;
      path->index = startIndex;
    }
};

}  // namespace linguine
