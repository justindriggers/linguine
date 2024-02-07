#pragma once

#include "Scene.h"

#include "ServiceLocator.h"

namespace linguine {

class GameOverScene : public Scene {
  public:
    GameOverScene(ServiceLocator& serviceLocator, int32_t points, uint8_t lives)
        : Scene(serviceLocator), _points(points), _lives(lives) {}

    void init() override;

  private:
    int32_t _points;
    uint8_t _lives;
};

}  // namespace linguine
