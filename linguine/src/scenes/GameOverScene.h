#pragma once

#include "Scene.h"

#include "ServiceLocator.h"
#include "data/LevelDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class GameOverScene : public Scene {
  public:
    GameOverScene(ServiceLocator& serviceLocator, uint8_t level)
        : Scene(serviceLocator), _level(level) {}

    void init() override;

  private:
    uint8_t _level;
    LevelDatabase _levelDatabase;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
