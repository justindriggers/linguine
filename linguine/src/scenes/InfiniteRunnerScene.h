#pragma once

#include "Scene.h"

#include <random>

#include "ServiceLocator.h"
#include "data/spells/SpellDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"
#include "physics/World.h"

namespace linguine {

class InfiniteRunnerScene : public Scene {
  public:
    explicit InfiniteRunnerScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())),
          _world(getEntityManager()) {}

    void init() override;

  private:
    std::random_device _random;
    UpgradeDatabase _upgradeDatabase;
    std::unique_ptr<SpellDatabase> _spellDatabase;
    physics::World _world;
};

}  // namespace linguine
