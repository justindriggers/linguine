#include "RunnerScene.h"

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Attachment.h"
#include "components/Boost.h"
#include "components/CameraFixture.h"
#include "components/Cast.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Emitter.h"
#include "components/Fire.h"
#include "components/Follow.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Particle.h"
#include "components/Party.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Score.h"
#include "components/ShipPart.h"
#include "components/SpawnPoint.h"
#include "components/StarSpawnPoint.h"
#include "components/Text.h"
#include "components/Toast.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Velocity.h"
#include "data/Palette.h"
#include "systems/AttachmentSystem.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CastSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CompletionSystem.h"
#include "systems/CooldownProgressSystem.h"
#include "systems/EventSystem.h"
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
#include "systems/ShakeSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/ToastSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/VelocitySystem.h"

namespace linguine {

void RunnerScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<VelocitySystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<PlayerControllerSystem>(getEntityManager(), get<InputManager>(), get<AudioManager>()));
  registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
  registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
  registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
  registerSystem(std::make_unique<HudSystem>(getEntityManager(), get<Renderer>(), get<SaveManager>()));
  registerSystem(std::make_unique<HealthProgressSystem>(getEntityManager()));
  registerSystem(std::make_unique<LivenessSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>(), get<SaveManager>(), get<ServiceLocator>()));
  registerSystem(std::make_unique<CompletionSystem>(getEntityManager(), get<ServiceLocator>()));
  registerSystem(std::make_unique<CooldownProgressSystem>(getEntityManager()));
  registerSystem(std::make_unique<CastSystem>(getEntityManager()));
  registerSystem(std::make_unique<ParticleSystem>(getEntityManager()));
  registerSystem(std::make_unique<FireSystem>(getEntityManager()));
  registerSystem(std::make_unique<ToastSystem>(getEntityManager()));
  registerSystem(std::make_unique<ShakeSystem>(getEntityManager(), get<SaveManager>()));
  registerSystem(std::make_unique<EventSystem>(getEntityManager()));

  // Scene-specific
  registerSystem(std::make_unique<SpawnSystem>(getEntityManager(), get<Renderer>(), *_spellDatabase));
  registerSystem(std::make_unique<ScoringSystem>(getEntityManager(), *_spellDatabase, get<Renderer>(), get<AudioManager>(), get<SaveManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();
  auto& saveManager = get<SaveManager>();

  {
    auto cameraEntity = createEntity();

    auto follow = cameraEntity->add<Follow>();

    switch (saveManager.getHandedness()) {
    case SaveManager::Handedness::Right:
      follow->offset = { 1.6f, 1.81578947368421f };
      break;
    case SaveManager::Handedness::Left:
      follow->offset = { -1.6f, 1.81578947368421f };
      break;
    }

    auto transform = cameraEntity->add<Transform>();
    transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    cameraEntity->add<PhysicalState>();
    cameraEntity->add<CircleCollider>()->radius = 40.0f;
    cameraEntity->add<Trigger>();

    auto fixture = cameraEntity->add<CameraFixture>();
    fixture->size = 15.2f;
    fixture->shake = true;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Blue;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });

    {
      auto spawnPointEntity = createEntity();

      auto spawnPoint = spawnPointEntity->add<SpawnPoint>();
      spawnPoint->distance = 6.25f;
      spawnPoint->lastSpawnPoint = 20.0f;
      spawnPoint->spawnChance = _config.spawnChance;
      spawnPoint->obstacleSpawnChance = _config.obstacleSpawnChance;
      spawnPoint->wallSpawnChance = _config.wallSpawnChance;
      spawnPoint->requiredDistance = _config.requiredDistance;
      spawnPoint->powerUpInterval = _config.powerUpInterval;
      spawnPoint->speedBoostEnabled = _config.speedBoostEnabled;
      spawnPoint->regenEnabled = _config.regenEnabled;
      spawnPoint->reviveEnabled = _config.reviveEnabled;
      spawnPoint->timeWarpEnabled = _config.timeWarpEnabled;

      auto starSpawnPoint = spawnPointEntity->add<StarSpawnPoint>();
      starSpawnPoint->lastSpawnPoint = -20.0f;

      spawnPointEntity->add<Transform>();
      spawnPointEntity->add<PhysicalState>();

      auto attachment = spawnPointEntity->add<Attachment>();
      attachment->parentId = cameraEntity->getId();
      attachment->offset = { 0.0f, 20.0f };
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

  auto level = saveManager.getCurrentLevel();
  auto shieldCount = _upgradeDatabase.getRankByLevel(0, level);

  {
    auto playerEntity = createEntity();

    auto player = playerEntity->add<Player>();
    player->speed = 3.0f + 1.0f * static_cast<float>(_upgradeDatabase.getRankByLevel(2, level));
    player->acceleration = 0.05f + 0.02f * static_cast<float>(_upgradeDatabase.getRankByLevel(3, level));
    player->maxSpeed = 30.0f;

    auto transform = playerEntity->add<Transform>();
    transform->scale = glm::vec3(2.5f);
    transform->position.z = 0.1f;

    playerEntity->add<PhysicalState>();

    auto circleCollider = playerEntity->add<CircleCollider>();
    circleCollider->radius = transform->scale.x / 2.0f;

    playerEntity->add<Velocity>();
    playerEntity->add<Trigger>();

    {
      auto shipEntity = createEntity();
      shipEntity->add<ShipPart>();

      auto shipTransform = shipEntity->add<Transform>();
      shipTransform->scale = transform->scale;
      shipTransform->position.z = 0.1f;

      shipEntity->add<PhysicalState>();

      auto attachment = shipEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();

      auto shipDrawable = shipEntity->add<Drawable>();
      shipDrawable->feature = new ColoredFeature();
      shipDrawable->feature->meshType = Ship;
      shipDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(shipDrawable->feature));
      shipDrawable.setRemovalListener([shipDrawable](const Entity e) {
        shipDrawable->renderable->destroy();
      });
    }

    {
      auto wingEntity = createEntity();
      wingEntity->add<ShipPart>();

      auto wingTransform = wingEntity->add<Transform>();
      wingTransform->scale = transform->scale;
      wingTransform->position.z = 1.0f;

      wingEntity->add<PhysicalState>();

      auto attachment = wingEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();

      auto wingDrawable = wingEntity->add<Drawable>();
      wingDrawable->feature = new ColoredFeature();
      wingDrawable->feature->meshType = Wing;
      wingDrawable->feature->color = Palette::Gray;
      wingDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(wingDrawable->feature));
      wingDrawable.setRemovalListener([wingDrawable](const Entity e) {
        wingDrawable->renderable->destroy();
      });
    }

    {
      auto cockpitEntity = createEntity();
      cockpitEntity->add<ShipPart>();

      auto cockpitTransform = cockpitEntity->add<Transform>();
      cockpitTransform->scale = transform->scale;
      cockpitTransform->position.z = 0.05f;

      cockpitEntity->add<PhysicalState>();

      auto attachment = cockpitEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();

      auto cockpitDrawable = cockpitEntity->add<Drawable>();
      cockpitDrawable->feature = new ColoredFeature();
      cockpitDrawable->feature->meshType = Cockpit;
      cockpitDrawable->feature->color = Palette::Blue;
      cockpitDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(cockpitDrawable->feature));
      cockpitDrawable.setRemovalListener([cockpitDrawable](const Entity e) {
        cockpitDrawable->renderable->destroy();
      });
    }

    {
      auto boosterEntity = createEntity();
      boosterEntity->add<ShipPart>();

      auto boosterTransform = boosterEntity->add<Transform>();
      boosterTransform->scale = transform->scale;
      boosterTransform->position.z = 0.05f;

      boosterEntity->add<PhysicalState>();

      auto attachment = boosterEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();

      auto boosterDrawable = boosterEntity->add<Drawable>();
      boosterDrawable->feature = new ColoredFeature();
      boosterDrawable->feature->meshType = Booster;
      boosterDrawable->feature->color = Palette::Black;
      boosterDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(boosterDrawable->feature));
      boosterDrawable.setRemovalListener([boosterDrawable](const Entity e) {
        boosterDrawable->renderable->destroy();
      });

      auto playerEntityId = playerEntity->getId();

      auto emitter = boosterEntity->add<Emitter>([this, playerEntityId, &renderer]() {
        auto playerEntity = getEntityManager().getById(playerEntityId);

        auto particleEntity = createEntity();

        auto particle = particleEntity->add<Particle>();
        particle->duration = 5.0f;

        auto playerTransform = playerEntity->get<Transform>();

        auto randomX = std::uniform_real_distribution(-0.2f * playerTransform->scale.x, 0.2f * playerTransform->scale.y);

        auto transform = particleEntity->add<Transform>();
        transform->position = { playerTransform->position.x + randomX(_random), playerTransform->position.y - 0.45f * playerTransform->scale.y, 2.0f };

        particleEntity->add<PhysicalState>(transform->position, 0.0f);

        auto velocity = particleEntity->add<Velocity>();
        velocity->velocity = { 0.0f, -3.0f };

        auto circle = particleEntity->add<Circle>();
        circle->feature = new CircleFeature();

        if (playerEntity->has<Boost>()) {
          auto boost = playerEntity->get<Boost>();

          particle->scalePerSecond = -3.0f * (boost->elapsed / boost->duration) - 1.0f;

          auto randomScale = std::uniform_real_distribution((0.125f + 0.075f * (1.0f - boost->elapsed / boost->duration)) * playerTransform->scale.x, (0.25f + 0.075f * (1.0f - boost->elapsed / boost->duration)) * playerTransform->scale.x);
          transform->scale = glm::vec3(randomScale(_random));

          circle->feature->color = Palette::Orange;
        } else {
          particle->scalePerSecond = -0.35f;

          auto randomScale = std::uniform_real_distribution(0.125f * playerTransform->scale.x, 0.25f * playerTransform->scale.x);
          transform->scale = glm::vec3(randomScale(_random));

          auto randomColor = std::uniform_int_distribution(0, 1);

          if (randomColor(_random) > 0) {
            circle->feature->color = Palette::Gray;
          }
        }

        circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
        circle.setRemovalListener([circle](const Entity e) {
          circle->renderable->destroy();
        });
      });
      emitter->frequency = 0.035f;
    }

    {
      auto engineFireEntity = createEntity();
      engineFireEntity->add<ShipPart>();
      engineFireEntity->add<Fire>();

      auto engineFireTransform = engineFireEntity->add<Transform>();
      engineFireTransform->position.z = 1.0f;

      engineFireEntity->add<PhysicalState>();

      auto attachment = engineFireEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();
      attachment->offset = { -0.13f * transform->scale.x, -0.24f * transform->scale.y};

      auto circle = engineFireEntity->add<Circle>();
      circle->feature = new CircleFeature();
      circle->feature->color = Palette::Orange;
      circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
      circle.setRemovalListener([circle](const Entity e) {
        circle->renderable->destroy();
      });
    }

    {
      auto engineFireEntity = createEntity();
      engineFireEntity->add<ShipPart>();
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
      circle->feature->color = Palette::Orange;
      circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
      circle.setRemovalListener([circle](const Entity e) {
        circle->renderable->destroy();
      });
    }

    auto party = playerEntity->add<Party>();

    for (auto i = 0; i < shieldCount; ++i) {
      auto shieldEntity = createEntity();

      shieldEntity->add<Health>(1000 + 250 * _upgradeDatabase.getRankByLevel(1, level));
      shieldEntity->add<Alive>();

      party->memberIds.push_back(shieldEntity->getId());
    }
  }

  // Shields Text
  {
    auto textEntity = createEntity();

    auto transform = textEntity->add<Transform>();
    transform->position = { 0.0f, (static_cast<float>(shieldCount) / 2.0f - static_cast<float>(shieldCount - 1) - 0.5f) * 46.0f - 51.0f, 0.0f };
    transform->scale = { 5.0f, 5.0f, 0.0f };

    switch (saveManager.getHandedness()) {
    case SaveManager::Handedness::Right:
      transform->position.x = 77.0f;
      break;
    case SaveManager::Handedness::Left:
      transform->position.x = -107.0f;
      break;
    }

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
    transform->position = { 0.0f, -23.0f, 0.0f};
    transform->rotation = glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    transform->scale = { static_cast<float>(shieldCount) * 40.0f + static_cast<float>(shieldCount - 1) * 6.0f, 4.0f, 0.0f };

    switch (saveManager.getHandedness()) {
    case SaveManager::Handedness::Right:
      transform->position.x = 118.0f;
      break;
    case SaveManager::Handedness::Left:
      transform->position.x = -118.0f;
      break;
    }

    auto progressable = healEntity->add<Progressable>();
    progressable->feature = new ProgressFeature();
    progressable->feature->meshType = Quad;
    progressable->feature->color = { 0.0f, 1.0f, 0.0f };
    progressable->feature->backgroundColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
    progressable.setRemovalListener([progressable](const Entity e) {
      progressable->renderable->destroy();
    });
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

    auto score = scoreEntity->add<Score>();
    score->level = _config.level;

    auto transform = scoreEntity->add<Transform>();
    transform->position = { -102.0f, 148.0f, 0.0f };
    transform->scale = glm::vec3(20.0f);

    auto text = scoreEntity->add<Text>();
    text->feature = new TextFeature();
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity e) {
      text->renderable->destroy();
    });
  }

  {
    auto toastEntity = createEntity();
    toastEntity->add<Transform>();

    auto toast = toastEntity->add<Toast>();
    toast->startPosition = { 0.0f, 1.81578947368421f * 240.0f / 15.2f, 0.0f };

    auto text = toastEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
    text->renderable->setEnabled(false);
  }

  auto& audioManager = get<AudioManager>();

  if (audioManager.getCurrentSongType() != SongType::Theme) {
    audioManager.play(SongType::Theme, Mode::Repeat);
  }
}

}  // namespace linguine
