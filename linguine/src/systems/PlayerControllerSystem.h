#pragma once

#include "System.h"

#include "data/spells/SpellDatabase.h"
#include "input/InputManager.h"

namespace linguine {

class PlayerControllerSystem : public System {
  public:
    explicit PlayerControllerSystem(EntityManager& entityManager,
                                    SpellDatabase& spellDatabase,
                                    InputManager& inputManager)
        : System(entityManager), _spellDatabase(spellDatabase),
          _inputManager(inputManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SpellDatabase& _spellDatabase;
    InputManager& _inputManager;
};

}  // namespace linguine
