#pragma once

#include "System.h"

#include "renderer/Renderer.h"

namespace linguine {

class CameraSystem : public System {
  public:
    CameraSystem(EntityManager& entityManager, Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    constexpr static float _height = 10.0f;

    Renderer& _renderer;
};

}  // namespace linguine