#pragma once

#include "RunnerScene.h"

namespace linguine {

class LevelNineScene : public RunnerScene {
  public:
    explicit LevelNineScene(ServiceLocator& serviceLocator)
        : RunnerScene(serviceLocator, Config{ 9, 0.9f, 0.0f, 0.0f, 5.0f, 1000.0f, false, true, false, false }) {}

    void init() override;

  private:
    std::random_device _random;
};

}  // namespace linguine
