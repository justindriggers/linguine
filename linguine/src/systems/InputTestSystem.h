#pragma once

#include "System.h"

#include "InputManager.h"
#include "Logger.h"
#include "renderer/Renderer.h"

namespace linguine {

class InputTestSystem : public System {
  public:
    explicit InputTestSystem(EntityManager& entityManager, Logger& logger,
                             InputManager& inputManager, Renderer& renderer)
        : System(entityManager), _logger(logger),
          _inputManager(inputManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Logger& _logger;
    InputManager& _inputManager;
    Renderer& _renderer;
};

}  // namespace linguine
