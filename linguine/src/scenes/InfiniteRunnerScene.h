#pragma once

#include "Scene.h"

#include <random>

#include "ServiceLocator.h"
#include "data/spells/SpellDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class InfiniteRunnerScene : public Scene {
  public:
    explicit InfiniteRunnerScene(ServiceLocator& serviceLocator);

  private:
    std::random_device _random;
    std::unique_ptr<SpellDatabase> _spellDatabase;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
