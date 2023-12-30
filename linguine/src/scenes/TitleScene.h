#pragma once

#include "Scene.h"

#include "InfiniteRunnerScene.h"
#include "ServiceLocator.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Dialog.h"
#include "components/Drawable.h"
#include "components/Footer.h"
#include "components/Selectable.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/DialogSystem.h"
#include "systems/FooterSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

class TitleScene : public Scene {
  public:
    explicit TitleScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator.get<EntityManagerFactory>().create()) {
      registerSystem(std::make_unique<FpsSystem>(getEntityManager(), serviceLocator.get<Logger>()));
      registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), serviceLocator.get<InputManager>(), serviceLocator.get<Renderer>(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<VelocitySystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraFollowSystem>(getEntityManager()));
      registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
      registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), serviceLocator.get<TimeManager>()));
      registerSystem(std::make_unique<CollisionSystem>(getEntityManager()));
      registerSystem(std::make_unique<ParticleSystem>(getEntityManager()));
      registerSystem(std::make_unique<FireSystem>(getEntityManager()));

      registerSystem(std::make_unique<SpawnSystem>(getEntityManager(), serviceLocator.get<Renderer>()));
      registerSystem(std::make_unique<DialogSystem>(getEntityManager()));
      registerSystem(std::make_unique<FooterSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
      registerSystem(std::make_unique<CameraSystem>(getEntityManager(), serviceLocator.get<Renderer>()));

      auto& renderer = serviceLocator.get<Renderer>();
      auto& sceneManager = serviceLocator.get<SceneManager>();
      auto& saveManager = serviceLocator.get<SaveManager>();

      {
        auto cameraEntity = createEntity();
        cameraEntity->add<Transform>();
        cameraEntity->add<PhysicalState>();
        cameraEntity->add<CircleCollider>()->radius = 20.0f;
        cameraEntity->add<Trigger>();

        auto fixture = cameraEntity->add<CameraFixture>();
        fixture->size = 12.0f;
        fixture->type = CameraFixture::Measurement::Width;
        fixture->camera = renderer.createCamera();
        fixture->camera->clearColor = Palette::Blue;
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

      {
        auto playerEntity = createEntity();

        auto transform = playerEntity->add<Transform>();
        transform->scale = glm::vec3(4.0f);

        playerEntity->add<PhysicalState>(transform->position, 0.0f);

        auto offset = glm::vec2(0.0f, 2.5f);

        auto player = playerEntity->add<Player>();
        player->speed = 2.0f + 1.0f * static_cast<float>(saveManager.getRank(2));
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
          shipDrawable->feature->meshType = Ship;
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
          attachment->offset = offset;

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
          attachment->offset = offset;

          auto boosterDrawable = boosterEntity->add<Drawable>();
          boosterDrawable->feature = new ColoredFeature();
          boosterDrawable->feature->meshType = Booster;
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
              circle->feature->color = { 0.78354f, 0.78354f, 0.78354f };
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
            sceneManager.load(std::make_unique<InfiniteRunnerScene>(serviceLocator));
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

      auto buttonPosition = -32.0f;

      if (!saveManager.isNewPlayer()) {
        auto playButtonEntity = createEntity();

        auto button = playButtonEntity->add<Button>();
        button->position = { 0.0f, buttonPosition, 5.0f };
        button->minSize = { 128.0f, 32.0f };
        button->text = "Play";
        button->textSize = 12.0f;
        button->clickHandler = [&sceneManager, &serviceLocator]() {
          sceneManager.load(std::make_unique<InfiniteRunnerScene>(serviceLocator));
        };

        buttonPosition -= 40.0f;
      }

      {
        auto newGameButtonEntity = createEntity();

        auto button = newGameButtonEntity->add<Button>();

        if (!saveManager.isNewPlayer()) {
          button->color = Palette::Secondary;
          button->activeColor = Palette::SecondaryAccent;
        }

        button->position = { 0.0f, buttonPosition, 5.0f };
        button->minSize = { 128.0f, 32.0f };
        button->text = "New Game";
        button->textSize = 12.0f;
        button->clickHandler = [this, &saveManager, &sceneManager, &serviceLocator]() {
          if (!saveManager.isNewPlayer()) {
            getEntityManager().find<Dialog>()->each([](const Entity& entity) {
              entity.get<Dialog>()->enabled = true;
            });
          } else {
            sceneManager.load(std::make_unique<InfiniteRunnerScene>(serviceLocator));
          }
        };

        buttonPosition -= 40.0f;
      }

      {
        auto optionsButtonEntity = createEntity();

        auto button = optionsButtonEntity->add<Button>();
        button->color = Palette::Secondary;
        button->activeColor = Palette::SecondaryAccent;
        button->position = { 0.0f, buttonPosition, 5.0f };
        button->minSize = { 128.0f, 32.0f };
        button->text = "Options";
        button->textSize = 12.0f;
        button->clickHandler = []() {
          printf("Click!");
        };
      }

      {
        auto footerPanelEntity = createEntity();
        footerPanelEntity->add<Footer>();

        auto transform = footerPanelEntity->add<Transform>();
        transform->position = { 0.0f, 0.0f, 10.0f };
        transform->scale = { 240.0f, 48.0f, 1.0f };

        footerPanelEntity->add<PhysicalState>(transform->position, 0.0f);

        auto drawable = footerPanelEntity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->color = Palette::SecondaryAccent;
        drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
        drawable.setRemovalListener([drawable](const Entity& e) {
          drawable->renderable->destroy();
        });

        {
          auto footerTextEntity = createEntity();

          auto footerTextTransform = footerTextEntity->add<Transform>();
          footerTextTransform->position = { 0.0f, 0.0f, 5.0f };
          footerTextTransform->scale = { 8.0f, 8.0f, 1.0f };

          footerTextEntity->add<PhysicalState>(footerTextTransform->position, 0.0f);

          auto attachment = footerTextEntity->add<Attachment>();
          attachment->parentId = footerPanelEntity->getId();
          attachment->offset = { -56.0f, 8.0f };

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
          footerTextTransform->scale = { 5.0f, 5.0f, 1.0f };

          footerTextEntity->add<PhysicalState>(footerTextTransform->position, 0.0f);

          auto attachment = footerTextEntity->add<Attachment>();
          attachment->parentId = footerPanelEntity->getId();
          attachment->offset = { -72.5f, -8.0f };

          auto text = footerTextEntity->add<Text>();
          text->feature = new TextFeature();
          text->feature->text = "Copyright 2023 Justin Driggers";
          text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
          text.setRemovalListener([text](const Entity& e) {
            text->renderable->destroy();
          });
        }
      }
    }

  private:
    std::random_device _random;
};

}  // namespace linguine
