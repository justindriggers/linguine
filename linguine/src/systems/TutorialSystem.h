#pragma once

#include "System.h"

#include "SaveManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class TutorialSystem : public System {
  public:
    TutorialSystem(EntityManager& entityManager,
                   SaveManager& saveManager,
                   Renderer& renderer)
        : System(entityManager), _saveManager(saveManager),
          _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SaveManager& _saveManager;
    Renderer& _renderer;
};

}  // namespace linguine
