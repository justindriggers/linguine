#pragma once

#include "Scene.h"

#include <random>

#include "components/Ability.h"
#include "components/AbilityLabel.h"
#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Emitter.h"
#include "components/Fire.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Particle.h"
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
#include "systems/FireSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/HealthProgressSystem.h"
#include "systems/HudSystem.h"
#include "systems/LivenessSystem.h"
#include "systems/ParticleSystem.h"
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
      registerSystem(std::make_unique<LivenessSystem>(getEntityManager(), serviceLocator.get<SaveManager>(), serviceLocator));
      registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
      registerSystem(std::make_unique<CastSystem>(getEntityManager()));
      registerSystem(std::make_unique<ParticleSystem>(getEntityManager()));
      registerSystem(std::make_unique<FireSystem>(getEntityManager()));

      // Scene-specific
      registerSystem(std::make_unique<SpawnSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<ScoringSystem>(getEntityManager(), *_spellDatabase));

      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      auto& saveManager = serviceLocator.get<SaveManager>();

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
        player->speed = 5.0f + 1.0f * saveManager.getRank(2);
        player->acceleration = 0.05f + 0.02f * saveManager.getRank(3);
        player->maxSpeed = 20.0f;

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(2.0f);
        transform->position.z = 0.1f;

        playerEntity->add<PhysicalState>();

        auto circleCollider = playerEntity->add<CircleCollider>();
        circleCollider->radius = transform->scale.x / 2.0f;

        playerEntity->add<Velocity>();
        playerEntity->add<Trigger>();

        auto drawable = playerEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Ship;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });

        {
          auto wingEntity = createEntity();

          auto wingTransform = wingEntity->add<Transform>();
          wingTransform->scale = transform->scale;
          wingTransform->position.z = 1.0f;

          wingEntity->add<PhysicalState>();

          auto attachment = wingEntity->add<Attachment>();
          attachment->parentId = playerEntity->getId();

          auto wingDrawable = wingEntity->add<Drawable>();
          wingDrawable->feature = new ColoredFeature();
          wingDrawable->feature->meshType = Wing;
          wingDrawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
          wingDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(wingDrawable->feature));
          wingDrawable.setRemovalListener([wingDrawable](const Entity e) {
            wingDrawable->renderable->destroy();
          });
        }

        {
          auto cockpitEntity = createEntity();

          auto cockpitTransform = cockpitEntity->add<Transform>();
          cockpitTransform->scale = transform->scale;

          cockpitEntity->add<PhysicalState>();

          auto attachment = cockpitEntity->add<Attachment>();
          attachment->parentId = playerEntity->getId();

          auto cockpitDrawable = cockpitEntity->add<Drawable>();
          cockpitDrawable->feature = new ColoredFeature();
          cockpitDrawable->feature->meshType = Cockpit;
          cockpitDrawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
          cockpitDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(cockpitDrawable->feature));
          cockpitDrawable.setRemovalListener([cockpitDrawable](const Entity e) {
            cockpitDrawable->renderable->destroy();
          });
        }

        {
          auto boosterEntity = createEntity();

          auto boosterTransform = boosterEntity->add<Transform>();
          boosterTransform->scale = transform->scale;

          boosterEntity->add<PhysicalState>();

          auto attachment = boosterEntity->add<Attachment>();
          attachment->parentId = playerEntity->getId();

          auto boosterDrawable = boosterEntity->add<Drawable>();
          boosterDrawable->feature = new ColoredFeature();
          boosterDrawable->feature->meshType = Booster;
          boosterDrawable->feature->color = { 0.0f, 0.0f, 0.0f };
          boosterDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(boosterDrawable->feature));
          boosterDrawable.setRemovalListener([boosterDrawable](const Entity e) {
            boosterDrawable->renderable->destroy();
          });
        }

        {
          auto engineFireEntity = createEntity();
          engineFireEntity->add<Fire>();

          auto engineFireTransform = engineFireEntity->add<Transform>();
          engineFireTransform->position.z = 1.0f;

          engineFireEntity->add<PhysicalState>();

          auto attachment = engineFireEntity->add<Attachment>();
          attachment->parentId = playerEntity->getId();
          attachment->offset = { -0.13f * transform->scale.x, -0.24f * transform->scale.y};

          auto circle = engineFireEntity->add<Circle>();
          circle->feature = new CircleFeature();
          circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
          circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
          circle.setRemovalListener([circle](const Entity e) {
            circle->renderable->destroy();
          });
        }

        {
          auto engineFireEntity = createEntity();
          engineFireEntity->add<Fire>();

          auto engineFireTransform = engineFireEntity->add<Transform>();
          engineFireTransform->scale = glm::vec3(0.25f);
          engineFireTransform->position.z = 1.0f;

          engineFireEntity->add<PhysicalState>();

          auto attachment = engineFireEntity->add<Attachment>();
          attachment->parentId = playerEntity->getId();
          attachment->offset = { 0.13f * transform->scale.x, -0.24f * transform->scale.y};

          auto circle = engineFireEntity->add<Circle>();
          circle->feature = new CircleFeature();
          circle->feature->color = { 0.97345f, 0.36625f, 0.00561f };
          circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
          circle.setRemovalListener([circle](const Entity e) {
            circle->renderable->destroy();
          });
        }

        auto party = playerEntity->add<Party>();

        for (auto i = 0; i < saveManager.getRank(0) + 1; ++i) {
          auto shieldEntity = createEntity();

          shieldEntity->add<Health>(1000 + 250 * saveManager.getRank(1));
          shieldEntity->add<Alive>();

          party->memberIds.push_back(shieldEntity->getId());
        }

        auto playerEntityId = playerEntity->getId();

        auto emitter = playerEntity->add<Emitter>([this, playerEntityId, &renderer]() {
          auto playerEntity = getEntityManager().getById(playerEntityId);

          auto particleEntity = createEntity();

          auto particle = particleEntity->add<Particle>();
          particle->duration = 5.0f;

          auto playerTransform = playerEntity->get<Transform>();

          auto randomScale = std::uniform_real_distribution(0.125f * playerTransform->scale.x, 0.25f * playerTransform->scale.x);
          auto randomX = std::uniform_real_distribution(-0.2f * playerTransform->scale.x, 0.2f * playerTransform->scale.y);

          auto transform = particleEntity->add<Transform>();
          transform->scale = glm::vec3(randomScale(_random));
          transform->position = { playerTransform->position.x + randomX(_random), playerTransform->position.y - 0.45f * playerTransform->scale.y, 2.0f };

          auto randomColor = std::uniform_int_distribution(0, 1);

          auto circle = particleEntity->add<Circle>();
          circle->feature = new CircleFeature();

          if (randomColor(_random) > 0) {
            circle->feature->color = { 0.78354f, 0.78354f, 0.78354f };
          }

          circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
          circle.setRemovalListener([circle](const Entity e) {
            circle->renderable->destroy();
          });
        });
        emitter->frequency = 0.035f;
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
    std::random_device _random;
    std::unique_ptr<SpellDatabase> _spellDatabase;
};

}  // namespace linguine
