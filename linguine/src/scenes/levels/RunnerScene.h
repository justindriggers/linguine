#pragma once

#include "Scene.h"

#include "data/spells/SpellDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class RunnerScene : public Scene {
  public:
    struct Config {
      uint8_t level = 1;
      float spawnChance = 0.85f;
      float obstacleSpawnChance = 0.65f;
      float wallSpawnChance = 0.0f;
      float powerUpInterval = 15.0f;
      float requiredDistance = 1000.0f;
      bool speedBoostEnabled = true;
      bool regenEnabled = true;
      bool reviveEnabled = true;
      bool timeWarpEnabled = true;
    };

    RunnerScene(ServiceLocator& serviceLocator, Config config)
        : Scene(serviceLocator), _config(config),
          _spellDatabase(std::make_unique<SpellDatabase>(serviceLocator, getEntityManager())) {}

    void init() override;

  private:
    std::unique_ptr<SpellDatabase> _spellDatabase;
    UpgradeDatabase _upgradeDatabase;
    std::random_device _random;

    Config _config;
};

}  // namespace linguine
