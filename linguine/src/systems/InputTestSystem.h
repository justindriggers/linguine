#pragma once

#include "System.h"

#include "InputManager.h"
#include "Logger.h"

namespace linguine {

class InputTestSystem : public System {
  public:
    explicit InputTestSystem(EntityManager& entityManager, Logger& logger,
                             InputManager& inputManager)
        : System(entityManager), _logger(logger),
          _inputManager(inputManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Logger& _logger;
    InputManager& _inputManager;
};

}  // namespace linguine
