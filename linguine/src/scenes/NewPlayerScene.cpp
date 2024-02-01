#include "NewPlayerScene.h"

#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Dialog.h"
#include "components/Drawable.h"
#include "components/Text.h"
#include "components/Toggle.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/DialogSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/ToggleSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void NewPlayerScene::init() {
  auto& saveManager = get<SaveManager>();
  auto& serviceLocator = get<ServiceLocator>();

  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<DialogSystem>(getEntityManager()));
  registerSystem(std::make_unique<ToggleSystem>(getEntityManager()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();

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
    auto handednessPanelEntity = createEntity();
    handednessPanelEntity->add<Dialog>()->enabled = true;

    auto transform = handednessPanelEntity->add<Transform>();
    transform->position = { 0.0f, 0.0f, 2.0f };
    transform->scale = { 200.0f, 96.0f, 1.0f };

    auto drawable = handednessPanelEntity->add<Drawable>();
    drawable->feature = new ColoredFeature();
    drawable->feature->color = Palette::White;
    drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
    drawable.setRemovalListener([drawable](const Entity& e) {
      drawable->renderable->destroy();
    });

    {
      auto headerPanelEntity = createEntity();
      headerPanelEntity->add<Dialog>()->enabled = true;

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
        headerTextEntity->add<Dialog>()->enabled = true;

        auto headerTextTransform = headerTextEntity->add<Transform>();
        headerTextTransform->position = { -86.0f, 36.0f, 0.0f };
        headerTextTransform->scale = { 8.0f, 8.0f, 1.0f };

        auto text = headerTextEntity->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = "Welcome";
        text->feature->color = Palette::White;
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity& e) {
          text->renderable->destroy();
        });
      }
    }

    {
      auto handednessDescriptionEntity = createEntity();
      handednessDescriptionEntity->add<Dialog>()->enabled = true;

      auto handednessDescriptionTransform = handednessDescriptionEntity->add<Transform>();
      handednessDescriptionTransform->position = { -72.0f, 0.0f, 0.0f };
      handednessDescriptionTransform->scale = { 6.0f, 6.0f, 1.0f };

      auto text = handednessDescriptionEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->text = "Which hand do you prefer?";
      text->feature->color = Palette::Secondary;
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });
    }

    {
      auto handednessToggle = createEntity();
      handednessToggle->add<Dialog>()->enabled = true;

      auto toggle = handednessToggle->add<Toggle>();
      toggle->position = { 0.0f, -26.0f, 1.0f };
      toggle->minSize = { 100.0f, 24.0f };
      toggle->textSize = 8.0f;
      toggle->offText = "Left";
      toggle->onText = "Right";
      toggle->stickySelection = true;

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
      auto continueButtonEntity = createEntity();
      continueButtonEntity->add<Dialog>()->enabled = true;

      auto continueButton = continueButtonEntity->add<Button>();
      continueButton->color = Palette::Secondary;
      continueButton->activeColor = Palette::SecondaryAccent;
      continueButton->position = { 0.0f, -64.0f, 1.0f };
      continueButton->minSize = { 200.0f, 32.0f };
      continueButton->text = "Continue";
      continueButton->textSize = 8.0f;
      continueButton->clickHandler = [this, &serviceLocator]() {
        _levelDatabase.load(serviceLocator, 1);
      };
    }

    {
      auto dialogBackgroundEntity = createEntity();
      dialogBackgroundEntity->add<Dialog>()->enabled = true;

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
}

}  // namespace linguine
