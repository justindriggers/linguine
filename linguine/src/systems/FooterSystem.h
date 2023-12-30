#pragma once

#include "System.h"

#include "renderer/Renderer.h"

namespace linguine {

class FooterSystem : public System {
  public:
    FooterSystem(EntityManager& entityManager, Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;
};

}  // namespace linguine
