#pragma once

#include "System.h"

#include "audio/AudioManager.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class LevelTrackingSystem : public System {
public:
  LevelTrackingSystem(EntityManager& entityManager,
                      AudioManager& audioManager)
      : System(entityManager), _audioManager(audioManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}

private:
  AudioManager& _audioManager;
  UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
