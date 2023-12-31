#pragma once

#include "System.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class LevelTrackingSystem : public System {
  public:
    LevelTrackingSystem(EntityManager& entityManager,
                        UpgradeDatabase& upgradeDatabase)
        : System(entityManager),
          _upgradeDatabase(upgradeDatabase) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    UpgradeDatabase& _upgradeDatabase;
};

}  // namespace linguine
