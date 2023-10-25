#pragma once

#include "System.h"

#include <random>

#include "data/spells/SpellDatabase.h"

namespace linguine {

class ScoringSystem : public System {
  public:
    explicit ScoringSystem(EntityManager& entityManager,
                           SpellDatabase& spellDatabase)
        : System(entityManager), _spellDatabase(spellDatabase) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    SpellDatabase& _spellDatabase;
};

}  // namespace linguine
