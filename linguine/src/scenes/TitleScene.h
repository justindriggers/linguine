#pragma once

#include "Scene.h"

#include <random>

#include "ServiceLocator.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class TitleScene : public Scene {
  public:
    explicit TitleScene(ServiceLocator& serviceLocator);

  private:
    std::random_device _random;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
