#include "GameOverScene.h"

#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void GameOverScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();
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
    auto missionLabelEntity = createEntity();

    auto transform = missionLabelEntity->add<Transform>();
    transform->position = { -48.0f, 128.0f, 0.0f };
    transform->scale = glm::vec3(16.0f);

    auto text = missionLabelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->feature->text = "Mission";
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  {
    auto failureLabelEntity = createEntity();

    auto transform = failureLabelEntity->add<Transform>();
    transform->position = { -60.0f, 88.0f, 0.0f };
    transform->scale = glm::vec3(24.0f);

    auto text = failureLabelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->feature->text = "Failed";
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  auto buttonPosition = -80.0f;

  {
    auto retryButtonEntity = createEntity();

    auto button = retryButtonEntity->add<Button>();
    button->position = { 0.0f, buttonPosition, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Retry";
    button->textSize = 12.0f;
    button->clickHandler = [this, &serviceLocator]() {
      _levelDatabase.load(serviceLocator, _level);
    };

    buttonPosition -= 40.0f;
  }

  {
    auto mainMenuButtonEntity = createEntity();

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

  auto& audioManager = get<AudioManager>();
  audioManager.play(SongType::GameOver, Mode::Once);
}

}  // namespace linguine
