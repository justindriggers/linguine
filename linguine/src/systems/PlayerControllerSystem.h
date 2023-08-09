#pragma once

#include "System.h"

#include "data/spells/SpellDatabase.h"

namespace linguine {

class PlayerControllerSystem : public System {
  public:
    explicit PlayerControllerSystem(EntityManager& entityManager,
                                    SpellDatabase& spellDatabase)
        : System(entityManager), _spellDatabase(spellDatabase) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SpellDatabase& _spellDatabase;
};

}  // namespace linguine
