#include "LevelCompletedScene.h"

#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/Event.h"
#include "components/PhysicalState.h"
#include "components/Shake.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/EventSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/ShakeSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void LevelCompletedScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));
  registerSystem(std::make_unique<ShakeSystem>(getEntityManager(), get<SaveManager>()));
  registerSystem(std::make_unique<EventSystem>(getEntityManager()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& audioManager = get<AudioManager>();
  auto& renderer = get<Renderer>();
  auto& saveManager = get<SaveManager>();
  auto& sceneManager = get<SceneManager>();
  auto& serviceLocator = get<ServiceLocator>();

  {
    auto uiCameraEntity = createEntity();
    uiCameraEntity->add<Transform>();
    uiCameraEntity->add<PhysicalState>();

    auto fixture = uiCameraEntity->add<CameraFixture>();
    fixture->size = 240.0f;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->shake = true;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Blue;
    fixture->camera->layer = UI;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });
  }

  auto topLabelEntity = createEntity();

  {
    auto transform = topLabelEntity->add<Transform>();
    transform->position = { -32.0f, 128.0f, 0.0f };
    transform->scale = glm::vec3(16.0f);

    auto text = topLabelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->feature->text = "Level";
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  auto bottomLabelEntity = createEntity();

  {
    auto transform = bottomLabelEntity->add<Transform>();
    transform->position = { -84.0f, 88.0f, 0.0f };
    transform->scale = glm::vec3(24.0f);

    auto text = bottomLabelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->feature->text = "Complete";
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  auto buttonPosition = -80.0f;

  auto nextButtonEntity = createEntity();

  {
    auto button = nextButtonEntity->add<Button>();
    button->position = { 0.0f, buttonPosition, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Level " + std::to_string(_level + 1);
    button->textSize = 12.0f;
    button->clickHandler = [this, &serviceLocator]() {
      _levelDatabase.load(serviceLocator, _level + 1);
    };

    buttonPosition -= 40.0f;
  }

  auto mainMenuButtonEntity = createEntity();

  {
    auto button = mainMenuButtonEntity->add<Button>();
    button->color = Palette::Secondary;
    button->activeColor = Palette::SecondaryAccent;
    button->position = { 0.0f, buttonPosition, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Main Menu";
    button->textSize = 12.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<TitleScene>(serviceLocator));
    };
  }

  auto starEntity1 = createEntity();

  {
    auto starEntityId = starEntity1->getId();

    auto transform = starEntity1->add<Transform>();
    transform->position = { -40.0f, 0.0f, 1.0f };
    transform->scale = glm::vec3(24.0f);

    auto circle = starEntity1->add<Circle>();
    circle->feature = new CircleFeature();
    circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
    circle.setRemovalListener([circle](const Entity& entity) {
      circle->renderable->destroy();
    });
    circle->renderable->setEnabled(false);

    if (_stars < 1) {
      circle->feature->color = Palette::Black;
    }

    auto eventEntity = createEntity();

    auto event = eventEntity->add<Event>();
    event->secondsRemaining = 0.5f;
    event->function = [this, starEntityId, &audioManager]() {
      auto shakeEntity = createEntity();

      if (_stars < 1) {
        shakeEntity->add<Shake>(0.5f, 0.5f);
        audioManager.play(EffectType::Collect1);
      } else {
        shakeEntity->add<Shake>(2.5f, 0.5f);
        audioManager.play(EffectType::Collect4);
      }

      auto starEntity = getEntityManager().getById(starEntityId);

      auto circle = starEntity->get<Circle>();
      circle->renderable->setEnabled(true);
    };
  }

  auto starEntity2 = createEntity();

  {
    auto starEntityId = starEntity2->getId();

    auto transform = starEntity2->add<Transform>();
    transform->position = { 0.0f, 0.0f, 1.0f };
    transform->scale = glm::vec3(24.0f);

    auto circle = starEntity2->add<Circle>();
    circle->feature = new CircleFeature();
    circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
    circle.setRemovalListener([circle](const Entity& entity) {
      circle->renderable->destroy();
    });
    circle->renderable->setEnabled(false);

    if (_stars < 2) {
      circle->feature->color = Palette::Black;
    }

    auto eventEntity = createEntity();

    auto event = eventEntity->add<Event>();
    event->secondsRemaining = 1.0f;
    event->function = [this, starEntityId, &audioManager]() {
      auto shakeEntity = createEntity();

      if (_stars < 2) {
        shakeEntity->add<Shake>(0.5f, 0.5f);
        audioManager.play(EffectType::Collect1);
      } else {
        shakeEntity->add<Shake>(5.0f, 0.5f);
        audioManager.play(EffectType::Collect5);
      }

      auto starEntity = getEntityManager().getById(starEntityId);

      auto circle = starEntity->get<Circle>();
      circle->renderable->setEnabled(true);
    };
  }

  auto starEntity3 = createEntity();

  {
    auto starEntityId = starEntity3->getId();

    auto transform = starEntity3->add<Transform>();
    transform->position = { 40.0f, 0.0f, 1.0f };
    transform->scale = glm::vec3(24.0f);

    auto circle = starEntity3->add<Circle>();
    circle->feature = new CircleFeature();
    circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
    circle.setRemovalListener([circle](const Entity& entity) {
      circle->renderable->destroy();
    });
    circle->renderable->setEnabled(false);

    if (_stars < 3) {
      circle->feature->color = Palette::Black;
    }

    auto eventEntity = createEntity();

    auto event = eventEntity->add<Event>();
    event->secondsRemaining = 1.5f;
    event->function = [this, starEntityId, &audioManager]() {
      auto shakeEntity = createEntity();

      if (_stars < 3) {
        shakeEntity->add<Shake>(0.5f, 0.5f);
        audioManager.play(EffectType::Collect1);
      } else {
        shakeEntity->add<Shake>(10.0f, 0.5f);
        audioManager.play(EffectType::Level);
      }

      auto starEntity = getEntityManager().getById(starEntityId);

      auto circle = starEntity->get<Circle>();
      circle->renderable->setEnabled(true);
    };
  }

  if (saveManager.getCurrentLevel() < _level + 1) {
    saveManager.ensureLevel(_level + 1);

    auto upgradeEntity = createEntity();

    auto upgradeState = upgradeEntity->add<UpgradeState>();
    upgradeState->upgrades = _upgradeDatabase.getDescriptionsByLevel(_level + 1);

    {
      auto continueButtonEntity = createEntity();

      auto nextButton = nextButtonEntity->get<Button>();
      nextButton->visible = false;

      auto mainMenuButton = mainMenuButtonEntity->get<Button>();
      mainMenuButton->visible = false;

      auto button = continueButtonEntity->add<Button>();
      button->position = { 0.0f, -80, 5.0f };
      button->minSize = { 128.0f, 32.0f };
      button->text = "Continue";
      button->textSize = 12.0f;
      button->clickHandler = [this, &audioManager, &renderer,
                              upgradeEntity, starEntity1, starEntity2, starEntity3,
                              continueButtonEntity, mainMenuButton, nextButton,
                              topLabelEntity, bottomLabelEntity]() {
        getEntityManager().find<UpgradeState>()->each([this, &audioManager, &renderer,
                                                       upgradeEntity, starEntity1, starEntity2, starEntity3,
                                                       continueButtonEntity, mainMenuButton, nextButton,
                                                       topLabelEntity, bottomLabelEntity](const Entity& entity) {
          auto upgradeState = entity.get<UpgradeState>();

          if (upgradeState->index == 0) {
            getEntityManager().find<Event>()->each([](Entity &entity) {
              entity.destroy();
            });

            starEntity1->destroy();
            starEntity2->destroy();
            starEntity3->destroy();

            auto topLabelText = topLabelEntity->get<Text>();
            topLabelText->feature->text = "Upgrade";

            auto topLabelTransform = topLabelEntity->get<Transform>();
            topLabelTransform->position.x = -48.0f;

            auto bottomLabelText = bottomLabelEntity->get<Text>();
            bottomLabelText->feature->text = "Unlocked";

            auto bottomLabelTransform = bottomLabelEntity->get<Transform>();
            bottomLabelTransform->position.x = -84.0f;

            {
              auto text = upgradeEntity->add<Text>();
              text->feature = new TextFeature();
              text->feature->color = Palette::White;
              text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
              text.setRemovalListener([text](const Entity& e) {
                text->renderable->destroy();
              });

              auto transform = upgradeEntity->add<Transform>();
              transform->scale = glm::vec3(12.0f);
            }
          }

          auto shakeEntity = createEntity();
          shakeEntity->add<Shake>(15.0f, 0.65f);

          audioManager.play(EffectType::PowerUp);

          auto upgradeText = upgradeEntity->get<Text>();
          upgradeText->feature->text = upgradeState->upgrades[upgradeState->index++];

          auto upgradeTransform = upgradeEntity->get<Transform>();
          upgradeTransform->position = { -(static_cast<float>(upgradeText->feature->text.size()) / 2.0f - 0.5f) * 12.0f, 0.0f, 0.0f };

          if (upgradeState->index == upgradeState->upgrades.size()) {
            continueButtonEntity->destroy();

            mainMenuButton->visible = true;
            nextButton->visible = true;
          }
        });
      };
    }
  }

  if (saveManager.getStarsForLevel(_level) < _stars) {
    saveManager.ensureStarsForLevel(_level, _stars);
  }
}

}  // namespace linguine
