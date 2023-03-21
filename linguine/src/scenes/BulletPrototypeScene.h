#pragma once

#include "Scene.h"

#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/PhysicalState.h"
#include "components/Rotating.h"
#include "components/SpawnPoint.h"
#include "components/Transform.h"
#include "factories/ProjectileFactory.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/FpsSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/ProjectileSpawnSystem.h"
#include "systems/ProjectileSystem.h"
#include "systems/RotatorSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class BulletPrototypeScene : public Scene {
  public:
    explicit BulletPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      _projectileFactory = std::make_unique<ProjectileFactory>(getEntityManager(), serviceLocator.get<Renderer>());

      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<ProjectileSpawnSystem>(getEntityManager(), *_projectileFactory));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ProjectileSystem>(getEntityManager()));
      registerSystem(std::make_unique<RotatorSystem>(getEntityManager(), serviceLocator.get<AudioManager>()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.03f, 0.04f, 0.07f);

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
        rotator->add<Transform>();
        rotator->add<PhysicalState>();

        auto spawnPoint = rotator->add<SpawnPoint>();
        spawnPoint->count = 36;
        spawnPoint->interval = 1.0f / 3.0f;

        auto rotating = rotator->add<Rotating>();
        rotating->speed = 0.07f;

        auto drawable = rotator->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }

      {
        auto ring = createEntity();
        ring->add<Transform>();
        ring->add<PhysicalState>();

        auto spawnPoint = ring->add<SpawnPoint>();
        spawnPoint->count = 72;
        spawnPoint->interval = 5.0f;
      }
    }

  private:
    std::unique_ptr<ProjectileFactory> _projectileFactory;
};

}  // namespace linguine
