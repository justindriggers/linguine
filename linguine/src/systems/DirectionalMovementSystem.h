#pragma once

#include "System.h"

#include "InputManager.h"

namespace linguine {

class DirectionalMovementSystem : public System {
  public:
    DirectionalMovementSystem(EntityManager& entityManager,
                              InputManager& inputManager)
        : System(entityManager), _inputManager(inputManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    InputManager& _inputManager;
};

}  // namespace linguine
