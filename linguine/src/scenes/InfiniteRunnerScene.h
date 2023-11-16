#pragma once

#include "Scene.h"

#include <sys/syslog.h>

#include "components/Ability.h"
#include "components/AbilityLabel.h"
#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Party.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Score.h"
#include "components/Selectable.h"
#include "components/SpawnPoint.h"
#include "components/TargetIndicator.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Unit.h"
#include "components/Velocity.h"
#include "systems/AttachmentSystem.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CastSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EffectSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/HudSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/ScoringSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/VelocitySystem.h"

namespace linguine {

class InfiniteRunnerScene : public Scene {
  public:
    explicit InfiniteRunnerScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager(), serviceLocator.get<InputManager>()));
      registerSystem(std::make_unique<VelocitySystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
      registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<EffectSystem>(getEntityManager(), *_spellDatabase));
      registerSystem(std::make_unique<HudSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager(), serviceLocator));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CastSystem>(getEntityManager()));

      // Scene-specific
      registerSystem(std::make_unique<SpawnSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<ScoringSystem>(getEntityManager(), *_spellDatabase));

      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();

      {
        auto cameraEntity = createEntity();

        auto transform = cameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        cameraEntity->add<PhysicalState>();
        cameraEntity->add<CircleCollider>()->radius = 20.0f;
        cameraEntity->add<Trigger>();

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->size = 12.0f;
        fixture->type = CameraFixture::Measurement::Width;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = { 0.007f, 0.01521f, 0.04667f };
        fixture.setRemovalListener([fixture](const Entity& e) {
          fixture->camera->destroy();
        });

        {
          auto spawnPointEntity = createEntity();

          auto spawnPoint = spawnPointEntity->add<SpawnPoint>();
          spawnPoint->distance = 5.0f;
          spawnPoint->spawnChance = 0.85f;

          auto spawnPointTransform = spawnPointEntity->add<Transform>();
          spawnPointTransform->position = { 0.0f, 15.0f, 0.0f };

          spawnPointEntity->add<PhysicalState>(spawnPointTransform->position, 0.0f);

          auto attachment = spawnPointEntity->add<Attachment>();
          attachment->parentId = cameraEntity->getId();
          attachment->offset = { 0.0f, 15.0f };
        }
      }

      {
        auto uiCameraEntity = createEntity();

        auto transform = uiCameraEntity->add<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        auto fixture = uiCameraEntity->add<CameraFixture>();
        fixture->size = 240.0f;
        fixture->type = CameraFixture::Measurement::Width;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = {};
        fixture->camera->layer = UI;
        fixture.setRemovalListener([fixture](const Entity& e) {
          fixture->camera->destroy();
        });
      }

      {
        auto playerEntity = createEntity();

        auto player = playerEntity->add<Player>();
        player->speed = 5.0f;
        player->acceleration = 0.05f;
        player->maxSpeed = 20.0f;

        playerEntity->add<Transform>();
        playerEntity->add<PhysicalState>();
        playerEntity->add<CircleCollider>();
        playerEntity->add<Velocity>();
        playerEntity->add<Trigger>();

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Triangle;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        auto party = playerEntity->add<Party>();

        auto shieldEntity = createEntity();

        shieldEntity->add<Health>(1000);
        shieldEntity->add<Alive>();

        party->memberIds.push_back(shieldEntity->getId());
      }

      // Shields Text
      {
        auto textEntity = createEntity();

        auto transform = textEntity->add<Transform>();
        transform->position = { -15.0f, -156.0f, 0.0f };
        transform->scale = { 5.0f, 5.0f, 0.0f };

        auto text = textEntity->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = "Shields";
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });
      }

      // Base Heal
      {
        auto healEntity = createEntity();
        healEntity->add<Ability>(_spellDatabase->getSpellById(1));

        auto transform = healEntity->add<Transform>();
        transform->position = { 0.0f, -96.0f, 0.0f};
        transform->scale = { 224.0f, 12.0f, 0.0f };

        auto progressable = healEntity->add<Progressable>();
        progressable->feature = new ProgressFeature();
        progressable->feature->meshType = Quad;
        progressable->feature->color = { 0.0f, 1.0f, 0.0f };
        progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
        progressable.setRemovalListener([progressable](const Entity e) {
          progressable->renderable->destroy();
        });

        // Text
        {
          auto textEntity = createEntity();
          textEntity->add<AbilityLabel>()->abilityId = healEntity->getId();

          auto textTransform = textEntity->add<Transform>();
          textTransform->position.x = -109.0f;
          textTransform->position.y = -80.0f;
          textTransform->scale = { 5.0f, 5.0f, 0.0f };

          auto text = textEntity->add<Text>();
          text->feature = new TextFeature();
          text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
          text.setRemovalListener([text](const Entity e) {
            text->renderable->destroy();
          });
        }
      }

      {
        auto playerCastEntity = createEntity();
        playerCastEntity->add<Friendly>();
        playerCastEntity->add<Cast>();
      }

      {
        auto playerGcdEntity = createEntity();
        playerGcdEntity->add<Friendly>();

        auto globalCooldown = playerGcdEntity->add<GlobalCooldown>();
        globalCooldown->total = 1.5f;
      }

      {
        auto scoreEntity = createEntity();

        scoreEntity->add<Score>();

        auto transform = scoreEntity->add<Transform>();
        transform->position = { -102.0f, 150.0f, 0.0f };
        transform->scale = glm::vec3(20.0f);

        auto text = scoreEntity->add<Text>();
        text->feature = new TextFeature();
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity e) {
          text->renderable->destroy();
        });
      }
    }

  private:
    std::unique_ptr<SpellDatabase> _spellDatabase;
};

}  // namespace linguine
