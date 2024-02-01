#include "LevelSelectionScene.h"

#include "NewPlayerScene.h"
#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void LevelSelectionScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

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
    transform->position = { -66.0f, 166.0f, 5.0f };
    transform->scale = glm::vec3(12.0f);

    auto text = headerTextEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Select Level";
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  auto x = -84.0f;
  auto y = 112.0f;

  for (auto i = 0; i < 5; ++i) {
    for (auto j = 0; j < 4; ++j) {
      auto levelSelectionEntity = createEntity();

      auto levelSelectionButton = levelSelectionEntity->add<Button>();
      levelSelectionButton->position = { x, y, 5.0f };
      levelSelectionButton->minSize = glm::vec2(40.0f);
      levelSelectionButton->textSize = 12.0f;

      auto level = i * 4 + j + 1;
      levelSelectionButton->text = std::to_string(level);

      if (level > saveManager.getCurrentLevel()) {
        levelSelectionButton->enabled = false;
        levelSelectionButton->color = Palette::SecondaryAccent;
        levelSelectionButton->activeColor = Palette::SecondaryAccent;
        levelSelectionButton->textColor = Palette::Gray;
      } else {
        auto stars = saveManager.getStarsForLevel(level);
        auto starX = x - (static_cast<float>(stars) / 2.0f - 0.5f) * 10.0f;

        for (auto star = 0; star < stars; ++star) {
          auto starEntity = createEntity();

          auto transform = starEntity->add<Transform>();
          transform->position = { starX, y - 20.0f, 3.0f };
          transform->scale = glm::vec3(6.0f);

          auto circle = starEntity->add<Circle>();
          circle->feature = new CircleFeature();
          circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
          circle.setRemovalListener([circle](const Entity& entity) {
            circle->renderable->destroy();
          });

          starX += 10.0f;
        }

        levelSelectionButton->clickHandler = [this, level, &saveManager, &sceneManager, &serviceLocator]() {
          if (saveManager.isNewPlayer()) {
            sceneManager.load(std::make_unique<NewPlayerScene>(serviceLocator));
            return;
          }

          _levelDatabase.load(serviceLocator, level);
        };
      }

      x += 56.0f;
    }

    x = -84.0f;
    y -= 56.0f;
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
