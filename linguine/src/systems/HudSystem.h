#pragma once

#include "System.h"

#include "SaveManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class HudSystem : public System {
  public:
    HudSystem(EntityManager& entityManager, Renderer& renderer,
              SaveManager& saveManager)
        : System(entityManager), _renderer(renderer),
          _saveManager(saveManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;
    SaveManager& _saveManager;
};

}  // namespace linguine
