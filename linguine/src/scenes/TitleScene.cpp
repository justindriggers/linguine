#include "TitleScene.h"

#include "InfiniteRunnerScene.h"
#include "LeaderboardScene.h"
#include "NewPlayerScene.h"
#include "OptionsScene.h"
#include "components/Attachment.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/CircleCollider.h"
#include "components/Dialog.h"
#include "components/Drawable.h"
#include "components/Emitter.h"
#include "components/ExternalLink.h"
#include "components/Fire.h"
#include "components/Follow.h"
#include "components/Footer.h"
#include "components/FooterPanel.h"
#include "components/LeaderboardButton.h"
#include "components/OptionsButton.h"
#include "components/Particle.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Selectable.h"
#include "components/ShipPart.h"
#include "components/StarSpawnPoint.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/Trigger.h"
#include "components/Velocity.h"
#include "data/Palette.h"
#include "data/upgrades/LevelCurve.h"
#include "systems/AttachmentSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraFollowSystem.h"
#include "systems/CameraSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/DialogSystem.h"
#include "systems/EdgeSystem.h"
#include "systems/ExternalLinkSystem.h"
#include "systems/FireSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/ParticleSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/TitleUiSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/VelocitySystem.h"

namespace linguine {

void TitleScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<VelocitySystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
  registerSystem(std::make_unique<DialogSystem>(getEntityManager()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));
  registerSystem(std::make_unique<EdgeSystem>(getEntityManager(), get<Renderer>()));
  registerSystem(std::make_unique<TitleUiSystem>(getEntityManager(), get<LeaderboardManager>()));
  registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
  registerSystem(std::make_unique<CollisionSystem>(getEntityManager(), _world));
  registerSystem(std::make_unique<ParticleSystem>(getEntityManager()));
  registerSystem(std::make_unique<FireSystem>(getEntityManager()));
  registerSystem(std::make_unique<ExternalLinkSystem>(getEntityManager(), get<UrlHandler>()));

  registerSystem(std::make_unique<SpawnSystem>(getEntityManager(), get<Renderer>(), *_spellDatabase));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& audioManager = get<AudioManager>();
  auto& renderer = get<Renderer>();
  auto& sceneManager = get<SceneManager>();
  auto& saveManager = get<SaveManager>();
  auto& serviceLocator = get<ServiceLocator>();

  {
    auto cameraEntity = createEntity();
    cameraEntity->add<Transform>();
    cameraEntity->add<PhysicalState>();
    cameraEntity->add<CircleCollider>()->radius = 20.0f;
    cameraEntity->add<Trigger>();
    cameraEntity->add<Follow>();

    auto fixture = cameraEntity->add<CameraFixture>();
    fixture->size = 12.0f;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Navy;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });

    {
      auto spawnPointEntity = createEntity();

      auto starSpawnPoint = spawnPointEntity->add<StarSpawnPoint>();
      starSpawnPoint->lastSpawnPoint = -15.0f;

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

  auto level = LevelCurve::getLevelForXp(saveManager.getPoints());

  {
    auto playerEntity = createEntity();

    auto transform = playerEntity->add<Transform>();
    transform->scale = glm::vec3(4.0f);

    playerEntity->add<PhysicalState>(transform->position, 0.0f);

    auto offset = glm::vec2(0.0f, 2.0f);

    auto player = playerEntity->add<Player>();
    player->maxSpeed = 10.0f + 2.5f * static_cast<float>(_upgradeDatabase.getRankByLevel(Upgrade::Type::Speed, level));
    player->speed = player->maxSpeed * 0.5f;
    player->acceleration = 0.0f;

    playerEntity->add<Velocity>();

    {
      auto shipEntity = createEntity();
      shipEntity->add<ShipPart>();

      auto shipTransform = shipEntity->add<Transform>();
      shipTransform->scale = transform->scale;
      shipTransform->position.z = 0.1f;

      shipEntity->add<PhysicalState>();

      auto attachment = shipEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();
      attachment->offset = offset;

      auto shipDrawable = shipEntity->add<Drawable>();
      shipDrawable->feature = new ColoredFeature();
      shipDrawable->feature->meshType = MeshType::Ship;
      shipDrawable->feature->color = Palette::White;
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
      attachment->offset = offset;

      auto wingDrawable = wingEntity->add<Drawable>();
      wingDrawable->feature = new ColoredFeature();
      wingDrawable->feature->meshType = MeshType::Wing;
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
      attachment->offset = offset;

      auto cockpitDrawable = cockpitEntity->add<Drawable>();
      cockpitDrawable->feature = new ColoredFeature();
      cockpitDrawable->feature->meshType = MeshType::Cockpit;
      cockpitDrawable->feature->color = Palette::Navy;
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
      attachment->offset = offset;

      auto boosterDrawable = boosterEntity->add<Drawable>();
      boosterDrawable->feature = new ColoredFeature();
      boosterDrawable->feature->meshType = MeshType::Booster;
      boosterDrawable->feature->color = Palette::Black;
      boosterDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(boosterDrawable->feature));
      boosterDrawable.setRemovalListener([boosterDrawable](const Entity e) {
        boosterDrawable->renderable->destroy();
      });

      auto playerEntityId = playerEntity->getId();

      auto emitter = boosterEntity->add<Emitter>([this, playerEntityId, offset, &renderer]() {
        auto playerEntity = getEntityManager().getById(playerEntityId);

        auto particleEntity = createEntity();

        auto particle = particleEntity->add<Particle>();
        particle->duration = 5.0f;

        auto playerTransform = playerEntity->get<Transform>();

        auto randomScale = std::uniform_real_distribution(0.125f * playerTransform->scale.x, 0.25f * playerTransform->scale.x);
        auto randomX = std::uniform_real_distribution(-0.2f * playerTransform->scale.x, 0.2f * playerTransform->scale.y);

        auto transform = particleEntity->add<Transform>();
        transform->scale = glm::vec3(randomScale(_random));
        transform->position = { playerTransform->position.x + randomX(_random) + offset.x, playerTransform->position.y - 0.45f * playerTransform->scale.y + offset.y, 2.0f };

        particleEntity->add<PhysicalState>(transform->position, 0.0f);

        auto velocity = particleEntity->add<Velocity>();
        velocity->velocity = { 0.0f, -3.0f };

        auto randomColor = std::uniform_int_distribution(0, 1);

        auto circle = particleEntity->add<Circle>();
        circle->feature = new CircleFeature();

        if (randomColor(_random) > 0) {
          circle->feature->color = Palette::Gray;
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
      attachment->offset = { -0.13f * transform->scale.x + offset.x, -0.24f * transform->scale.y + offset.y };

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
      engineFireTransform->position.z = 1.0f;

      engineFireEntity->add<PhysicalState>();

      auto attachment = engineFireEntity->add<Attachment>();
      attachment->parentId = playerEntity->getId();
      attachment->offset = { 0.13f * transform->scale.x + offset.x, -0.24f * transform->scale.y + offset.y };

      auto circle = engineFireEntity->add<Circle>();
      circle->feature = new CircleFeature();
      circle->feature->color = Palette::Orange;
      circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature));
      circle.setRemovalListener([circle](const Entity e) {
        circle->renderable->destroy();
      });
    }
  }

  {
    auto titleTextEntity = createEntity();

    auto transform = titleTextEntity->add<Transform>();
    transform->position = { -64.0f, 144.0f, 5.0f };
    transform->scale = { 32.0f, 32.0f, 1.0f };

    auto text = titleTextEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Aegis";
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  {
    auto titleTextEntity = createEntity();

    auto transform = titleTextEntity->add<Transform>();
    transform->position = { -56.0f, 112.0f, 5.0f };
    transform->scale = glm::vec3(16.0f);

    auto text = titleTextEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Infinite";
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  {
    auto confirmationPanelEntity = createEntity();
    confirmationPanelEntity->add<Dialog>();

    auto transform = confirmationPanelEntity->add<Transform>();
    transform->position = { 0.0f, 0.0f, 2.0f };
    transform->scale = { 200.0f, 96.0f, 1.0f };

    auto drawable = confirmationPanelEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->color = Palette::White;
    drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
    drawable.setRemovalListener([drawable](const Entity& e) {
      drawable->renderable->destroy();
    });

    {
      auto headerPanelEntity = createEntity();
      headerPanelEntity->add<Dialog>();

      auto headerTransform = headerPanelEntity->add<Transform>();
      headerTransform->position = { 0.0f, 36.0f, 1.0f };
      headerTransform->scale = { 200.0f, 24.0f, 1.0f };

      auto headerDrawable = headerPanelEntity->add<Drawable>();
      headerDrawable->feature = new ColoredFeature();
      headerDrawable->feature->color = Palette::SecondaryAccent;
      headerDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(headerDrawable->feature), UI);
      headerDrawable.setRemovalListener([headerDrawable](const Entity& e) {
        headerDrawable->renderable->destroy();
      });

      {
        auto headerTextEntity = createEntity();
        headerTextEntity->add<Dialog>();

        auto headerTextTransform = headerTextEntity->add<Transform>();
        headerTextTransform->position = { -86.0f, 36.0f, 0.0f };
        headerTextTransform->scale = { 8.0f, 8.0f, 1.0f };

        auto text = headerTextEntity->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = "Are you sure?";
        text->feature->color = Palette::White;
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity& e) {
          text->renderable->destroy();
        });
      }
    }

    {
      auto confirmationDescriptionEntity = createEntity();
      confirmationDescriptionEntity->add<Dialog>();

      auto confirmationDescriptionTransform = confirmationDescriptionEntity->add<Transform>();
      confirmationDescriptionTransform->position = { -72.0f, 0.0f, 0.0f };
      confirmationDescriptionTransform->scale = { 6.0f, 6.0f, 1.0f };

      auto text = confirmationDescriptionEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->text = "All progress will be lost";
      text->feature->color = Palette::Secondary;
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });
    }

    {
      auto cancelButtonEntity = createEntity();
      cancelButtonEntity->add<Dialog>();

      auto cancelButton = cancelButtonEntity->add<Button>();
      cancelButton->color = Palette::Secondary;
      cancelButton->activeColor = Palette::SecondaryAccent;
      cancelButton->position = { -50.0f, -36.0f, 1.0f };
      cancelButton->minSize = { 100.0f, 32.0f };
      cancelButton->text = "Cancel";
      cancelButton->textSize = 8.0f;
      cancelButton->clickHandler = [this]() {
        getEntityManager().find<Dialog>()->each([](const Entity& entity) {
          entity.get<Dialog>()->enabled = false;
        });
      };
    }

    {
      auto confirmButtonEntity = createEntity();
      confirmButtonEntity->add<Dialog>();

      auto confirmButton = confirmButtonEntity->add<Button>();
      confirmButton->position = { 50.0f, -36.0f, 1.0f };
      confirmButton->minSize = { 100.0f, 32.0f };
      confirmButton->text = "Confirm";
      confirmButton->textSize = 8.0f;
      confirmButton->clickHandler = [&saveManager, &sceneManager, &serviceLocator]() {
        saveManager.restart();
        sceneManager.load(std::make_unique<NewPlayerScene>(serviceLocator));
      };
    }

    {
      auto dialogBackgroundEntity = createEntity();
      dialogBackgroundEntity->add<Dialog>();

      auto dialogBackgroundTransform = dialogBackgroundEntity->add<Transform>();
      dialogBackgroundTransform->position = { 0.0f, 0.0f, 3.0f };
      dialogBackgroundTransform->scale = glm::vec3(1000.0f);

      auto dialogBackgroundDrawable = dialogBackgroundEntity->add<Drawable>();
      dialogBackgroundDrawable->feature = new ColoredFeature();
      dialogBackgroundDrawable->feature->color = Palette::Black;
      dialogBackgroundDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(dialogBackgroundDrawable->feature), UI);
      dialogBackgroundDrawable.setRemovalListener([dialogBackgroundDrawable](const Entity& e) {
        dialogBackgroundDrawable->renderable->destroy();
      });
    }
  }

  {
    auto newGameButtonEntity = createEntity();

    auto button = newGameButtonEntity->add<Button>();
    button->position = { 0.0f, -32.0f, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Play";
    button->textSize = 12.0f;
    button->clickHandler = [&saveManager, &sceneManager, &serviceLocator]() {
      saveManager.restart();

      if (saveManager.isNewPlayer()) {
        sceneManager.load(std::make_unique<NewPlayerScene>(serviceLocator));
      } else {
        sceneManager.load(std::make_unique<InfiniteRunnerScene>(serviceLocator));
      }
    };
  }

  {
    auto leaderboardButtonEntity = createEntity();
    leaderboardButtonEntity->add<LeaderboardButton>();

    auto button = leaderboardButtonEntity->add<Button>();
    button->color = Palette::Secondary;
    button->activeColor = Palette::SecondaryAccent;
    button->position = { 0.0f, -72.0f, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Leaderboard";
    button->textSize = 10.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<LeaderboardScene>(serviceLocator));
    };
    button->visible = false;
  }

  {
    auto optionsButtonEntity = createEntity();
    optionsButtonEntity->add<OptionsButton>();

    auto button = optionsButtonEntity->add<Button>();
    button->color = Palette::Secondary;
    button->activeColor = Palette::SecondaryAccent;
    button->position = { 0.0f, -112.0f, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Options";
    button->textSize = 10.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<OptionsScene>(serviceLocator));
    };
  }

  {
    auto footerEntity = createEntity();
    footerEntity->add<Transform>();

    auto footer = footerEntity->add<Footer>();
    footer->ignoreInset = true;
    footer->offset = 16.0f;

    {
      auto footerPanelEntity = createEntity();

      auto footerPanel = footerPanelEntity->add<FooterPanel>();
      footerPanel->padding = 38.0f;

      auto transform = footerPanelEntity->add<Transform>();
      transform->position = { 0.0f, 0.0f, 10.0f };
      transform->scale = { 240.0f, 48.0f, 1.0f };

      auto drawable = footerPanelEntity->add<Drawable>();
      drawable->feature = new ColoredFeature();
      drawable->feature->color = Palette::SecondaryAccent;
      drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
      drawable.setRemovalListener([drawable](const Entity& e) {
        drawable->renderable->destroy();
      });

      auto selectable = footerPanelEntity->add<Selectable>();
      selectable->feature = new SelectableFeature();
      selectable->feature->entityId = footerPanelEntity->getId();
      selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature), UI);
      selectable.setRemovalListener([selectable](const Entity& e) {
        selectable->renderable->destroy();
      });

      auto externalLink = footerPanelEntity->add<ExternalLink>();
      externalLink->url = "https://log.kravick.dev";
    }

    {
      auto footerTextEntity = createEntity();

      auto footerTextTransform = footerTextEntity->add<Transform>();
      footerTextTransform->position = { 0.0f, 0.0f, 5.0f };
      footerTextTransform->scale = glm::vec3(8.0f);

      auto attachment = footerTextEntity->add<Attachment>();
      attachment->parentId = footerEntity->getId();
      attachment->offset = { -56.0f, 18.5f };
      attachment->useFixedUpdate = false;

      auto text = footerTextEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->text = "log.kravick.dev";
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });
    }

    {
      auto footerTextEntity = createEntity();

      auto footerTextTransform = footerTextEntity->add<Transform>();
      footerTextTransform->position = { 0.0f, 0.0f, 5.0f };
      footerTextTransform->scale = glm::vec3(5.0f);

      auto attachment = footerTextEntity->add<Attachment>();
      attachment->parentId = footerEntity->getId();
      attachment->offset = { -75.0f, 2.5f };
      attachment->useFixedUpdate = false;

      auto text = footerTextEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->text = "Copyright 2023-2024 Kravick LLC";
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });
    }
  }

  if (audioManager.getCurrentSongType() != SongType::Title) {
    audioManager.play(SongType::Title, Mode::Repeat);
  }
}

}  // namespace linguine
