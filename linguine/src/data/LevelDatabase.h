#pragma once

#include "ServiceLocator.h"
#include "scenes/levels/LevelNineScene.h"
#include "scenes/levels/RunnerScene.h"

namespace linguine {

class LevelDatabase {
  public:
    void load(ServiceLocator& serviceLocator, uint8_t level) {
      std::unique_ptr<Scene> newScene;

      switch (level) {
      case 1:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.0f, 0.0f, 0.0f, 135.0f, false, false, false, false });
        break;
      case 2:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.95f, 0.0f, 0.0f, 0.0f, 185.625f, false, false, false, false });
        break;
      case 3:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.25f, 0.0f, 0.0f, 185.625f, false, false, false, false });
        break;
      case 4:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.5f, 0.0f, 0.0f, 185.625f, false, false, false, false });
        break;
      case 5:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.65f, 0.0f, 0.0f, 330.0f, false, false, false, false });
        break;
      case 6:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.0f, 0.0f, 15.0f, 425.0f, true, false, false, false });
        break;
      case 7:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.65f, 0.0f, 15.0f, 750.0f, true, false, false, false });
        break;
      case 8:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.25f, 0.0f, 2.0f, 1000.0f, true, false, false, false });
        break;
      case 9:
        newScene = std::make_unique<LevelNineScene>(serviceLocator);
        break;
      case 10:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.15f, 0.15f, 2.0f, 1000.0f, false, true, false, false });
        break;
      case 11:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.65f, 0.0f, 15.0f, 1500.0f, true, true, false, false });
        break;
      case 15:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.65f, 0.0f, 15.0f, 2000.0f, true, true, true, false });
        break;
      case 16:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.65f, 0.0f, 15.0f, 2000.0f, true, true, true, true });
        break;
      case 19:
        newScene = std::make_unique<RunnerScene>(serviceLocator, RunnerScene::Config{ level, 0.85f, 0.0f, 0.0f, 15.0f, 5000.0f, true, true, true, true });
        break;
      default:
        break;
      }

      auto& sceneManager = serviceLocator.get<SceneManager>();
      sceneManager.load(std::move(newScene));
    }
};

}  // namespace linguine
