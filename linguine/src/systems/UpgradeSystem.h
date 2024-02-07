#pragma once

#include "System.h"

#include "SaveManager.h"
#include "audio/AudioManager.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class UpgradeSystem : public System {
  public:
    UpgradeSystem(EntityManager& entityManager,
                  AudioManager& audioManager,
                  SaveManager& saveManager)
        : System(entityManager), _audioManager(audioManager),
          _saveManager(saveManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    struct UpgradeMetadata {
      uint8_t knownLevel = 0;
    };

    AudioManager& _audioManager;
    SaveManager& _saveManager;
    UpgradeDatabase _upgradeDatabase;
};

}  // namespace linguine
