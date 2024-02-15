#pragma once

#include "System.h"

#include <random>

#include "components/SpawnPoint.h"
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

    bool spawnPowerUp(Component<SpawnPoint>& point);

    void spawnAsteroid(float y, int size = 0, int position = -1);

    void spawnObstacle(float x, float y);

    void spawnStars(float y);
};

}  // namespace linguine
