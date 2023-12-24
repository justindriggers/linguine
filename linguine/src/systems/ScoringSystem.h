#pragma once

#include "System.h"

#include <random>

#include "data/spells/SpellDatabase.h"

namespace linguine {

class ScoringSystem : public System {
  public:
    explicit ScoringSystem(EntityManager& entityManager,
                           SpellDatabase& spellDatabase,
                           Renderer& renderer)
        : System(entityManager), _spellDatabase(spellDatabase),
          _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    SpellDatabase& _spellDatabase;
    Renderer& _renderer;
};

}  // namespace linguine
