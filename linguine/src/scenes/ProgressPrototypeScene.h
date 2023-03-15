#pragma once

#include "Scene.h"

#include "components/Alive.h"
#include "components/BigHeal.h"
#include "components/CameraFixture.h"
#include "components/Cooldown.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Progressable.h"
#include "components/Selectable.h"
#include "components/Transform.h"
#include "systems/CameraSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EnemyAttackSystem.h"
#include "systems/EnemySpawnSystem.h"
#include "systems/FpsSystem.h"
#include "systems/FriendlyAttackSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class ProgressPrototypeScene : public Scene {
  public:
    explicit ProgressPrototypeScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager()));
      registerSystem(std::make_unique<EnemySpawnSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<EnemyAttackSystem>(getEntityManager()));
      registerSystem(std::make_unique<FriendlyAttackSystem>(getEntityManager()));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      renderer.getCamera().clearColor = glm::vec3(0.13f, 0.23f, 0.97f);

      auto cameraEntity = createEntity();
      cameraEntity->add<CameraFixture>();
      cameraEntity->add<Transform>();

      createFriendly(renderer, glm::vec3(0.0f));
      createFriendly(renderer, glm::vec3(-2.0f, 0.0f, 0.0f));
      createFriendly(renderer, glm::vec3(2.0f, 0.0f, 0.0f));

      {
        // Global Cooldown Indicator
        auto entity = createEntity();

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(0.0f, -3.5f, 0.0f);

        auto progressable = entity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.88f, 0.34f, 0.1f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });
        progressable->renderable->setEnabled(false);

        auto globalCooldown = entity->add<GlobalCooldown>();
        globalCooldown->elapsed = 1.5f;
        globalCooldown->total = 1.5f;
      }

      {
        // Big Heal
        auto entity = createEntity();

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(0.0f, -5.5f, 0.0f);
        transform->scale = glm::vec3(1.25f);

        auto progressable = entity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = glm::vec3(0.0f, 1.0f, 0.0f);
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        auto selectable = entity->add<Selectable>();
        selectable->feature = new SelectableFeature();
        selectable->feature->entityId = entity->getId();
        selectable->feature->meshType = Quad;
        selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
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
    void createFriendly(Renderer& renderer, glm::vec3 location) {
      auto entity = createEntity();
      entity->add<Friendly>();

      auto transform = entity->add<Transform>();
      transform->position = location;

      auto progressable = entity->add<Progressable>();
      progressable->feature = new ProgressFeature();
      progressable->feature->meshType = Quad;
      progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
      progressable.setRemovalListener([progressable](const Entity e) {
        progressable->renderable->destroy();
      });

      auto selectable = entity->add<Selectable>();
      selectable->feature = new SelectableFeature();
      selectable->feature->entityId = entity->getId();
      selectable->feature->meshType = Quad;
      selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
      selectable.setRemovalListener([selectable](const Entity e) {
        selectable->renderable->destroy();
      });

      auto health = entity->add<Health>();
      health->current = 1'000;
      health->max = 1'000;

      entity->add<Alive>();
    }
};

}  // namespace linguine
