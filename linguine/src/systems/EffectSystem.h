#pragma once

#include "System.h"

#include "data/spells/SpellDatabase.h"

namespace linguine {

class EffectSystem : public System {
  public:
    EffectSystem(EntityManager& entityManager,
                 SpellDatabase& spellDatabase)
        : System(entityManager), _spellDatabase(spellDatabase) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    SpellDatabase& _spellDatabase;
};

}  // namespace linguine
