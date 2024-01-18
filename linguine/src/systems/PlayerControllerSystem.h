#pragma once

#include "System.h"

#include "data/spells/SpellDatabase.h"
#include "input/InputManager.h"

namespace linguine {

class PlayerControllerSystem : public System {
  public:
    PlayerControllerSystem(EntityManager& entityManager,
                           InputManager& inputManager,
                           AudioManager& audioManager)
        : System(entityManager), _inputManager(inputManager),
          _audioManager(audioManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    InputManager& _inputManager;
    AudioManager& _audioManager;
};

}  // namespace linguine
