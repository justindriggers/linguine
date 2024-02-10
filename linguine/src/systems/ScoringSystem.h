#pragma once

#include "System.h"

#include <random>

#include "data/spells/SpellDatabase.h"
#include "data/upgrades/UpgradeDatabase.h"

namespace linguine {

class ScoringSystem : public System {
  public:
    ScoringSystem(EntityManager& entityManager,
                  SpellDatabase& spellDatabase,
                  Renderer& renderer,
                  AudioManager& audioManager,
                  SaveManager& saveManager)
        : System(entityManager), _spellDatabase(spellDatabase),
          _renderer(renderer), _audioManager(audioManager),
          _saveManager(saveManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;
    UpgradeDatabase _upgradeDatabase;

    SpellDatabase& _spellDatabase;
    Renderer& _renderer;
    AudioManager& _audioManager;
    SaveManager& _saveManager;
};

}  // namespace linguine
