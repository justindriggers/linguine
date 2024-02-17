#include "LeaderboardScene.h"

#include "TitleScene.h"
#include "components/Button.h"
#include "components/CameraFixture.h"
#include "components/Drawable.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "data/Palette.h"
#include "systems/ButtonSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsSystem.h"
#include "systems/GestureRecognitionSystem.h"
#include "systems/TransformationSystem.h"

namespace linguine {

void LeaderboardScene::init() {
  registerSystem(std::make_unique<FpsSystem>(getEntityManager(), get<Logger>()));
  registerSystem(std::make_unique<GestureRecognitionSystem>(getEntityManager(), get<InputManager>(), get<Renderer>(), get<TimeManager>()));
  registerSystem(std::make_unique<ButtonSystem>(getEntityManager(), get<Renderer>(), get<AudioManager>()));

  registerSystem(std::make_unique<TransformationSystem>(getEntityManager()));
  registerSystem(std::make_unique<CameraSystem>(getEntityManager(), get<Renderer>()));

  auto& renderer = get<Renderer>();
  auto& sceneManager = get<SceneManager>();
  auto& serviceLocator = get<ServiceLocator>();
  auto& leaderboardManager = get<LeaderboardManager>();

  {
    auto uiCameraEntity = createEntity();
    uiCameraEntity->add<Transform>();

    auto fixture = uiCameraEntity->add<CameraFixture>();
    fixture->size = 240.0f;
    fixture->type = CameraFixture::Measurement::Width;
    fixture->camera = renderer.createCamera();
    fixture->camera->clearColor = Palette::Navy;
    fixture->camera->layer = UI;
    fixture.setRemovalListener([fixture](const Entity& e) {
      fixture->camera->destroy();
    });
  }

  {
    auto headerTextEntity = createEntity();

    auto transform = headerTextEntity->add<Transform>();
    transform->position = { -60.0f, 180.0f, 5.0f };
    transform->scale = glm::vec3(12.0f);

    auto text = headerTextEntity->add<Text>();
    text->feature = new TextFeature();
    text->feature->text = "Leaderboard";
    text->feature->color = Palette::White;
    text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
    text.setRemovalListener([text](const Entity& e) {
      text->renderable->destroy();
    });
  }

  leaderboardManager.getHighScores(
      [this, &renderer](auto scores) {
        if (scores.empty()) {
          auto messageEntity = createEntity();

          auto transform = messageEntity->add<Transform>();
          transform->position = { 0.0f, 0.0f, 5.0f };
          transform->scale = glm::vec3(8.0f);

          auto text = messageEntity->add<Text>();
          text->feature = new TextFeature();
          text->feature->text = "No scores available";
          text->feature->color = Palette::PrimaryAccent;
          text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
          text.setRemovalListener([text](const Entity& e) {
            text->renderable->destroy();
          });

          transform->position.x -= (static_cast<float>(text->feature->text.length()) / 2.0f - 0.5f) * 8.0f;

          return;
        }

        auto top = 148.0f;

        for (auto i = 0; i < scores.size(); ++i) {
          auto& record = scores[i];

          int nameOffset;

          {
            auto rankEntity = createEntity();

            auto transform = rankEntity->add<Transform>();
            transform->position = { -102.0f, top - static_cast<float>(i) * 30.0f, 5.0f };
            transform->scale = glm::vec3(8.0f);

            auto text = rankEntity->add<Text>();
            text->feature = new TextFeature();
            text->feature->text = std::to_string(i + 1) + ".";
            text->feature->color = Palette::White;
            text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
            text.setRemovalListener([text](const Entity& e) {
              text->renderable->destroy();
            });

            nameOffset = static_cast<int>(text->feature->text.length() - 2);
          }

          {
            auto nameEntity = createEntity();

            auto transform = nameEntity->add<Transform>();
            transform->position = { -82.0f, top - static_cast<float>(i) * 30.0f, 5.0f };
            transform->scale = glm::vec3(8.0f);

            auto randomLength = std::uniform_int_distribution(3, 15);

            auto text = nameEntity->add<Text>();
            text->feature = new TextFeature();
            text->feature->text = record.name;
            text->feature->color = Palette::White;
            text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
            text.setRemovalListener([text](const Entity& e) {
              text->renderable->destroy();
            });

            transform->position.x += static_cast<float>(nameOffset) * 8.0f;
          }

          {
            auto scoreEntity = createEntity();

            auto transform = scoreEntity->add<Transform>();
            transform->position = { 102.0f, top - static_cast<float>(i) * 30.0f, 5.0f };
            transform->scale = glm::vec3(8.0f);

            auto text = scoreEntity->add<Text>();
            text->feature = new TextFeature();
            text->feature->text = std::to_string(record.score);

            if (record.isPlayer) {
              text->feature->color = Palette::White;
            } else {
              text->feature->color = Palette::Primary;
            }

            text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
            text.setRemovalListener([text](const Entity& e) {
              text->renderable->destroy();
            });

            transform->position.x -= static_cast<float>(text->feature->text.length() - 1) * 8.0f;
          }

          {
            auto backgroundEntity = createEntity();

            auto transform = backgroundEntity->add<Transform>();
            transform->position = { 0.0f, top - static_cast<float>(i) * 30.0f, 10.0f };
            transform->scale = { 224.0f, 30.0f, 1.0f };

            auto drawable = backgroundEntity->add<Drawable>();
            drawable->feature = new ColoredFeature();

            if (record.isPlayer) {
              drawable->feature->color = Palette::Primary;
            } else if (i % 2 == 0) {
              drawable->feature->color = Palette::Secondary;
            } else {
              drawable->feature->color = Palette::SecondaryAccent;
            }

            drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature), UI);
            drawable.setRemovalListener([drawable](const Entity& entity) {
              drawable->renderable->destroy();
            });
          }
        }
      },
      [this, &renderer](auto error) {
        auto errorEntity = createEntity();

        auto transform = errorEntity->add<Transform>();
        transform->position = { 0.0f, 0.0f, 5.0f };
        transform->scale = glm::vec3(8.0f);

        auto text = errorEntity->add<Text>();
        text->feature = new TextFeature();
        text->feature->text = error;
        text->feature->color = Palette::PrimaryAccent;
        text->renderable = renderer.create(std::unique_ptr<TextFeature>(text->feature), UI);
        text.setRemovalListener([text](const Entity& e) {
          text->renderable->destroy();
        });

        transform->position.x -= (static_cast<float>(text->feature->text.length()) / 2.0f - 0.5f) * 8.0f;
      }
  );

  {
    auto mainMenuButtonEntity = createEntity();

    auto button = mainMenuButtonEntity->add<Button>();
    button->color = Palette::Secondary;
    button->activeColor = Palette::SecondaryAccent;
    button->position = { 0.0f, -164.0f, 5.0f };
    button->minSize = { 208.0f, 32.0f };
    button->text = "Main Menu";
    button->textSize = 12.0f;
    button->clickHandler = [&sceneManager, &serviceLocator]() {
      sceneManager.load(std::make_unique<TitleScene>(serviceLocator));
    };
  }
}

}  // namespace linguine
