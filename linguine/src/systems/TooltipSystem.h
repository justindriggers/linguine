#pragma once

#include "System.h"

#include "input/InputManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class TooltipSystem : public System {
  public:
    TooltipSystem(EntityManager& entityManager, Renderer& renderer,
                  InputManager& inputManager)
        : System(entityManager), _renderer(renderer),
          _inputManager(inputManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;
    InputManager& _inputManager;
};

}  // namespace linguine
