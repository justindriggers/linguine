#include "GameOverScene.h"

#include "InfiniteRunnerScene.h"
#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/PhysicalState.h"
#include "components/Progressable.h"
#include "components/Text.h"
#include "components/Toast.h"
#include "components/Transform.h"
#include "components/gameover/CurrentLevel.h"
#include "components/gameover/CurrentXp.h"
#include "components/gameover/LevelTracker.h"
#include "components/gameover/RequiredXp.h"
#include "data/Palette.h"
#include "data/upgrades/LevelCurve.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/LevelTrackingSystem.h"
#include "systems/PhysicsInterpolationSystem.h"
#include "systems/ShakeSystem.h"
#include "systems/ToastSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void GameOverScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<PhysicsInterpolationSystem>(getEntityManager(), get<TimeManager>()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));
  registerSystem(std::make_unique<ToastSystem>(getEntityManager()));
  registerSystem(std::make_unique<ShakeSystem>(getEntityManager(), get<SaveManager>()));

  registerSystem(std::make_unique<LevelTrackingSystem>(getEntityManager(), get<AudioManager>(), _upgradeDatabase));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();
  auto& sceneManager = get<SceneManager>();
  auto& saveManager = get<SaveManager>();
  auto& serviceLocator = get<ServiceLocator>();

  {
    auto uiCameraEntity = createEntity();
    uiCameraEntity->add<Transform>();
    uiCameraEntity->add<PhysicalState>();

    auto fixture = uiCameraEntity->add<CameraFixture>();
    fixture->size = 240.0f;
    fixture->shake = true;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Blue;
    fixture->camera->layer = UI;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });
  }

  {
    auto levelEntity = createEntity();
    levelEntity->add<CurrentLevel>();

    auto text = levelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->color = Palette::White;
    text->feature->text = std::to_string(LevelCurve::getLevelForXp(saveManager.getPoints()));
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });

    auto levelTransform = levelEntity->add<Transform>();
    levelTransform->position = { 0.0f, 88.0f, 0.0f };
    levelTransform->position.x = -static_cast<float>(text->feature->text.size()) * 32.0f / 2.0f + 16.0f;
    levelTransform->scale = glm::vec3(32.0f);

    {
      auto levelLabelEntity = createEntity();

      auto levelLabelTransform = levelLabelEntity->add<Transform>();
      levelLabelTransform->position = { -32.0f, 128.0f, 0.0f };
      levelLabelTransform->scale = glm::vec3(16.0f);

      auto levelLabelText = levelLabelEntity->add<Text>();
      levelLabelText->feature = new TextFeature();
      levelLabelText->feature->color = Palette::White;
      levelLabelText->feature->text = "Level";
      levelLabelText->renderable = renderer.create(std::unique_ptr<TextFeature>(levelLabelText->feature), UI);
      levelLabelText.setRemovalListener([levelLabelText](const Entity& e) {
        levelLabelText->renderable->destroy();
      });
    }
  }

  {
    auto progressBarEntity = createEntity();
    progressBarEntity->add<LevelTracker>(saveManager.getPoints(), saveManager.getPoints() + _points);

    saveManager.addPoints(_points);

    auto transform = progressBarEntity->add<Transform>();
    transform->scale = { 192.0f, 12.0f, 1.0f };

    auto progressable = progressBarEntity->add<Progressable>();
    progressable->feature = new ProgressFeature();
    progressable->feature->color = Palette::Orange;
    progressable->renderable = renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature), UI);
    progressable.setRemovalListener([progressable](const Entity& entity) {
      progressable->renderable->destroy();
    });

    {
      auto currentXpEntity = createEntity();
      currentXpEntity->add<CurrentXp>();

      auto currentXpTransform = currentXpEntity->add<Transform>();
      currentXpTransform->position = { -88.0f, -16.0f, 0.0f };
      currentXpTransform->scale = glm::vec3(8.0f);

      auto currentXpText = currentXpEntity->add<Text>();
      currentXpText->feature = new TextFeature();
      currentXpText->feature->color = Palette::White;
      currentXpText->renderable = renderer.create(std::unique_ptr<TextFeature>(currentXpText->feature), UI);
      currentXpText.setRemovalListener([currentXpText](const Entity& e) {
        currentXpText->renderable->destroy();
      });
    }

    {
      auto requiredXpEntity = createEntity();
      requiredXpEntity->add<RequiredXp>();

      auto requiredXpTransform = requiredXpEntity->add<Transform>();
      requiredXpTransform->position = { 0.0f, -16.0f, 0.0f };
      requiredXpTransform->scale = glm::vec3(8.0f);

      auto requiredXpText = requiredXpEntity->add<Text>();
      requiredXpText->feature = new TextFeature();
      requiredXpText->feature->color = Palette::White;
      requiredXpText->renderable = renderer.create(std::unique_ptr<TextFeature>(requiredXpText->feature), UI);
      requiredXpText.setRemovalListener([requiredXpText](const Entity& e) {
        requiredXpText->renderable->destroy();
      });
    }
  }

  auto buttonPosition = -80.0f;

  {
    auto retryButtonEntity = createEntity();

    auto button = retryButtonEntity->add<Button>();
    button->position = { 0.0f, buttonPosition, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Retry";
    button->textSize = 12.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<InfiniteRunnerScene>(serviceLocator));
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

  {
    auto toastEntity = createEntity();
    toastEntity->add<Transform>();

    auto toast = toastEntity->add<Toast>();
    toast->startPosition = { 0.0f, 16.0f, 0.0f };

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
  audioManager.play(SongType::GameOver, Mode::Once);
}

}  // namespace linguine
