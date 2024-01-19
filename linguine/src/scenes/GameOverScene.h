#pragma once

#include "Scene.h"

#include "ServiceLocator.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class GameOverScene : public Scene {
  public:
    GameOverScene(ServiceLocator& serviceLocator, int32_t points)
        : Scene(serviceLocator), _points(points) {}

    void init() override;

  private:
    int32_t _points;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
