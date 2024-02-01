#include "OptionsScene.h"

#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Text.h"
#include "components/Toggle.h"
#include "components/Transform.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/ToggleSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void OptionsScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<ToggleSystem>(getEntityManager()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));

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

    auto fixture = uiCameraEntity->add<CameraFixture>();
    fixture->size = 240.0f;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Blue;
    fixture->camera->layer = UI;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });
  }

  {
    auto headerTextEntity = createEntity();

    auto transform = headerTextEntity->add<Transform>();
    transform->position = { -36.0f, 166.0f, 5.0f };
    transform->scale = glm::vec3(12.0f);

    auto text = headerTextEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Options";
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  {
    auto musicLabel = createEntity();

    auto musicLabelTransform = musicLabel->add<Transform>();
    musicLabelTransform->position = { -99.0f, 72.0f, 0.0f };
    musicLabelTransform->scale = glm::vec3(10.0f);

    auto musicLabelText = musicLabel->add<Text>();
    musicLabelText->feature = new TextFeature();
    musicLabelText->feature->color = Palette::White;
    musicLabelText->feature->text = "Music";
    musicLabelText->renderable = renderer.create(std::unique_ptr<TextFeature>(musicLabelText->feature), UI);
    musicLabelText.setRemovalListener([musicLabelText](const Entity& e) {
      musicLabelText->renderable->destroy();
    });

    auto musicToggle = createEntity();

    auto toggle = musicToggle->add<Toggle>();
    toggle->position = { 72.0f, 72.0f, 5.0f };
    toggle->minSize = { 72.0f, 24.0f };
    toggle->textSize = 8.0f;
    toggle->isEnabled = saveManager.isMusicEnabled();
    toggle->toggleHandler = [&saveManager, &audioManager](bool enabled) {
      saveManager.setMusicEnabled(enabled);
      audioManager.setMusicEnabled(enabled);

      if (enabled) {
        audioManager.play(SongType::Title, Mode::Repeat);
      }
    };
  }

  {
    auto sfxLabel = createEntity();

    auto sfxLabelTransform = sfxLabel->add<Transform>();
    sfxLabelTransform->position = { -99.0f, 24.0f, 0.0f };
    sfxLabelTransform->scale = glm::vec3(10.0f);

    auto sfxLabelText = sfxLabel->add<Text>();
    sfxLabelText->feature = new TextFeature();
    sfxLabelText->feature->color = Palette::White;
    sfxLabelText->feature->text = "Sound Effects";
    sfxLabelText->renderable = renderer.create(std::unique_ptr<TextFeature>(sfxLabelText->feature), UI);
    sfxLabelText.setRemovalListener([sfxLabelText](const Entity& e) {
      sfxLabelText->renderable->destroy();
    });

    auto sfxToggle = createEntity();

    auto toggle = sfxToggle->add<Toggle>();
    toggle->position = { 72.0f, 24.0f, 5.0f };
    toggle->minSize = { 72.0f, 24.0f };
    toggle->textSize = 8.0f;
    toggle->isEnabled = saveManager.isSoundEffectsEnabled();
    toggle->toggleHandler = [&saveManager, &audioManager](bool enabled) {
      saveManager.setSoundEffectsEnabled(enabled);
      audioManager.setSoundEffectsEnabled(enabled);
    };
  }

  {
    auto screenShakeLabel = createEntity();

    auto screenShakeLabelTransform = screenShakeLabel->add<Transform>();
    screenShakeLabelTransform->position = { -99.0f, -24.0f, 0.0f };
    screenShakeLabelTransform->scale = glm::vec3(10.0f);

    auto screenShakeLabelText = screenShakeLabel->add<Text>();
    screenShakeLabelText->feature = new TextFeature();
    screenShakeLabelText->feature->color = Palette::White;
    screenShakeLabelText->feature->text = "Screen Shake";
    screenShakeLabelText->renderable = renderer.create(std::unique_ptr<TextFeature>(screenShakeLabelText->feature), UI);
    screenShakeLabelText.setRemovalListener([screenShakeLabelText](const Entity& e) {
      screenShakeLabelText->renderable->destroy();
    });

    auto screenShakeToggle = createEntity();

    auto toggle = screenShakeToggle->add<Toggle>();
    toggle->position = { 72.0f, -24.0f, 5.0f };
    toggle->minSize = { 72.0f, 24.0f };
    toggle->textSize = 8.0f;
    toggle->isEnabled = saveManager.isScreenShakeEnabled();
    toggle->toggleHandler = [&saveManager](bool enabled) {
      saveManager.setScreenShakeEnabled(enabled);
    };
  }

  {
    auto handednessLabel = createEntity();

    auto handednessLabelTransform = handednessLabel->add<Transform>();
    handednessLabelTransform->position = { -99.0f, -72.0f, 0.0f };
    handednessLabelTransform->scale = glm::vec3(10.0f);

    auto handednessText = handednessLabel->add<Text>();
    handednessText->feature = new TextFeature();
    handednessText->feature->color = Palette::White;
    handednessText->feature->text = "Handedness";
    handednessText->renderable = renderer.create(std::unique_ptr<TextFeature>(handednessText->feature), UI);
    handednessText.setRemovalListener([handednessText](const Entity& e) {
      handednessText->renderable->destroy();
    });

    auto handednessToggle = createEntity();

    auto toggle = handednessToggle->add<Toggle>();
    toggle->position = { 72.0f, -72.0f, 5.0f };
    toggle->minSize = { 72.0f, 24.0f };
    toggle->textSize = 8.0f;
    toggle->offText = "L";
    toggle->onText = "R";

    switch (saveManager.getHandedness()) {
    case SaveManager::Handedness::Left:
      toggle->isEnabled = false;
      break;
    case SaveManager::Handedness::Right:
      toggle->isEnabled = true;
      break;
    }

    toggle->toggleHandler = [&saveManager](bool enabled) {
      if (enabled) {
        saveManager.setHandedness(SaveManager::Handedness::Right);
      } else {
        saveManager.setHandedness(SaveManager::Handedness::Left);
      }
    };
  }

  {
    auto backButtonEntity = createEntity();

    auto button = backButtonEntity->add<Button>();
    button->color = Palette::Secondary;
    button->activeColor = Palette::SecondaryAccent;
    button->position = { 0.0f, -164.0f, 5.0f };
    button->minSize = { 208.0f, 32.0f };
    button->text = "Back";
    button->textSize = 12.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<TitleScene>(serviceLocator));
    };
  }
}

}  // namespace linguine
