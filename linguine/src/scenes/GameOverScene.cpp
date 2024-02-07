#include "GameOverScene.h"

#include "InfiniteRunnerScene.h"
#include "TitleScene.h"
#include "components/Attachment.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Circle.h"
#include "components/Drawable.h"
#include "components/Header.h"
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
#include "systems/AttachmentSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/EdgeSystem.h"
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
  registerSystem(std::make_unique<EdgeSystem>(getEntityManager(), get<Renderer>()));
  registerSystem(std::make_unique<AttachmentSystem>(getEntityManager()));
  registerSystem(std::make_unique<ToastSystem>(getEntityManager()));
  registerSystem(std::make_unique<ShakeSystem>(getEntityManager(), get<SaveManager>()));

  registerSystem(std::make_unique<LevelTrackingSystem>(getEntityManager(), get<AudioManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();
  auto& sceneManager = get<SceneManager>();
  auto& saveManager = get<SaveManager>();
  saveManager.setLives(_lives);

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

  if (saveManager.getLives() > 0) {
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
      progressBarEntity->add<LevelTracker>(saveManager.getPoints(), _points);

      saveManager.setPoints(_points);

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
  } else {
    {
      auto gameEntity = createEntity();

      auto text = gameEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->color = Palette::White;
      text->feature->text = "Game";
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });

      auto transform = gameEntity->add<Transform>();
      transform->scale = glm::vec3(28.0f);
      transform->position = { -42.0f, 88.0f, 0.0f };
    }

    {
      auto overEntity = createEntity();

      auto text = overEntity->add<Text>();
      text->feature = new TextFeature();
      text->feature->color = Palette::White;
      text->feature->text = "Over";
      text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
      text.setRemovalListener([text](const Entity& e) {
        text->renderable->destroy();
      });

      auto transform = overEntity->add<Transform>();
      transform->scale = glm::vec3(32.0f);
      transform->position = { -48.0f, 42.0f, 0.0f };
    }
  }

  auto buttonPosition = -80.0f;

  if (saveManager.getLives() > 0) {
    auto retryButtonEntity = createEntity();

    auto button = retryButtonEntity->add<Button>();
    button->position = { 0.0f, buttonPosition, 5.0f };
    button->minSize = { 128.0f, 32.0f };
    button->text = "Continue";
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

  auto headerEntity = createEntity();
  headerEntity->add<Header>()->offset = 16.0f;
  headerEntity->add<Transform>();

  {
    auto levelEntity = createEntity();

    auto attachment = levelEntity->add<Attachment>();
    attachment->parentId = headerEntity->getId();
    attachment->offset = { -105.0f, 0.0f };
    attachment->useFixedUpdate = false;

    auto text = levelEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Lvl";
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity e) {
      text->renderable->destroy();
    });

    auto transform = levelEntity->add<Transform>();
    transform->scale = glm::vec3(6.0f);
  }

  {
    auto levelNumberEntity = createEntity();

    auto attachment = levelNumberEntity->add<Attachment>();
    attachment->parentId = headerEntity->getId();
    attachment->offset = { -82.0f, 0.0f };
    attachment->useFixedUpdate = false;

    auto text = levelNumberEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = std::to_string(LevelCurve::getLevelForXp(_points));
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity e) {
      text->renderable->destroy();
    });

    auto transform = levelNumberEntity->add<Transform>();
    transform->scale = glm::vec3(10.0f);
  }

  {
    auto lifeIndicatorEntity = createEntity();
    auto indicatorAttachment = lifeIndicatorEntity->add<Attachment>();

    {
      indicatorAttachment->parentId = headerEntity->getId();
      indicatorAttachment->offset = { 0.0f, -1.0f };
      indicatorAttachment->useFixedUpdate = false;

      auto transform = lifeIndicatorEntity->add<Transform>();
      transform->scale = glm::vec3(14.0f);

      {
        auto shipEntity = createEntity();

        auto shipTransform = shipEntity->add<Transform>();
        shipTransform->scale = transform->scale;
        shipTransform->position.z = 0.1f;

        shipEntity->add<PhysicalState>();

        auto shipAttachment = shipEntity->add<Attachment>();
        shipAttachment->parentId = lifeIndicatorEntity->getId();
        shipAttachment->useFixedUpdate = false;

        auto shipDrawable = shipEntity->add<Drawable>();
        shipDrawable->feature = new ColoredFeature();
        shipDrawable->feature->meshType = Ship;
        shipDrawable->feature->color = Palette::White;
        shipDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(shipDrawable->feature), UI);
        shipDrawable.setRemovalListener([shipDrawable](const Entity e) {
          shipDrawable->renderable->destroy();
        });
      }

      {
        auto wingEntity = createEntity();

        auto wingTransform = wingEntity->add<Transform>();
        wingTransform->scale = transform->scale;
        wingTransform->position.z = 1.0f;

        wingEntity->add<PhysicalState>();

        auto shipAttachment = wingEntity->add<Attachment>();
        shipAttachment->parentId = lifeIndicatorEntity->getId();
        shipAttachment->useFixedUpdate = false;

        auto wingDrawable = wingEntity->add<Drawable>();
        wingDrawable->feature = new ColoredFeature();
        wingDrawable->feature->meshType = Wing;
        wingDrawable->feature->color = Palette::White;
        wingDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(wingDrawable->feature), UI);
        wingDrawable.setRemovalListener([wingDrawable](const Entity e) {
          wingDrawable->renderable->destroy();
        });
      }

      {
        auto boosterEntity = createEntity();

        auto boosterTransform = boosterEntity->add<Transform>();
        boosterTransform->scale = transform->scale;
        boosterTransform->position.z = 0.05f;

        boosterEntity->add<PhysicalState>();

        auto shipAttachment = boosterEntity->add<Attachment>();
        shipAttachment->parentId = lifeIndicatorEntity->getId();
        shipAttachment->useFixedUpdate = false;

        auto boosterDrawable = boosterEntity->add<Drawable>();
        boosterDrawable->feature = new ColoredFeature();
        boosterDrawable->feature->meshType = Booster;
        boosterDrawable->feature->color = Palette::White;
        boosterDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(boosterDrawable->feature), UI);
        boosterDrawable.setRemovalListener([boosterDrawable](const Entity e) {
          boosterDrawable->renderable->destroy();
        });
      }
    }

    auto livesEntity = createEntity();

    auto attachment = livesEntity->add<Attachment>();
    attachment->parentId = headerEntity->getId();
    attachment->useFixedUpdate = false;

    auto transform = livesEntity->add<Transform>();
    transform->scale = glm::vec3(10.0f);

    auto text = livesEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = std::to_string(_lives);
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity e) {
      text->renderable->destroy();
    });

    attachment->offset.x = 103.0f - static_cast<float>(text->feature->text.size() - 1) * 10.0f;
    indicatorAttachment->offset.x = attachment->offset.x - 16.0f;
  }

  {
    auto scoreIndicatorEntity = createEntity();
    auto scoreIndicatorAttachment = scoreIndicatorEntity->add<Attachment>();

    {
      scoreIndicatorAttachment->parentId = headerEntity->getId();
      scoreIndicatorAttachment->useFixedUpdate = false;

      auto transform = scoreIndicatorEntity->add<Transform>();
      transform->scale = glm::vec3(10.0f);

      auto circle = scoreIndicatorEntity->add<Circle>();
      circle->feature = new CircleFeature();
      circle->feature->color = Palette::Orange;
      circle->renderable = renderer.create(std::unique_ptr<CircleFeature>(circle->feature), UI);
      circle.setRemovalListener([circle](const Entity& entity) {
        circle->renderable->destroy();
      });
    }

    auto scoreEntity = createEntity();

    auto attachment = scoreEntity->add<Attachment>();
    attachment->parentId = headerEntity->getId();
    attachment->useFixedUpdate = false;

    auto transform = scoreEntity->add<Transform>();
    transform->scale = glm::vec3(10.0f);

    auto text = scoreEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = std::to_string(_points);
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity e) {
      text->renderable->destroy();
    });

    attachment->offset.x = 8.0f - (static_cast<float>(text->feature->text.size()) / 2.0f - 0.5f) * 10.0f;
    scoreIndicatorAttachment->offset.x = attachment->offset.x - 16.0f;
  }

  {
    auto toastEntity = createEntity();

    auto transform = toastEntity->add<Transform>();
    transform->scale = glm::vec3(6.0f);

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