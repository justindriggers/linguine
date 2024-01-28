#pragma once

#include "System.h"

#include <random>

#include "data/spells/SpellDatabase.h"
#include "renderer/Renderer.h"

namespace linguine {

class SpawnSystem : public System {
  public:
    SpawnSystem(EntityManager& entityManager, Renderer& renderer,
                SpellDatabase& spellDatabase)
        : System(entityManager), _renderer(renderer),
          _spellDatabase(spellDatabase) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    Renderer& _renderer;
    SpellDatabase& _spellDatabase;

    void spawnPowerUp(float y, std::unordered_map<uint64_t, float>& cooldowns);

    void spawnAsteroid(float y, int size = 0);

    void spawnObstacles(float y);

    void spawnStars(float y);

    void spawnMovementText(float y);

    void spawnScoringText(float y);

    void spawnHealingText(float y);

    void spawnEvasionText(float y);
};

}  // namespace linguine
