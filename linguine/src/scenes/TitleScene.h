#pragma once

#include "Scene.h"

#include <random>

#include "ServiceLocator.h"
#include "data/spells/SpellDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class TitleScene : public Scene {
  public:
    explicit TitleScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())) {}

    void init() override;

  private:
    std::random_device _random;
    std::unique_ptr<SpellDatabase> _spellDatabase;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
