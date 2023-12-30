#pragma once

#include "System.h"

#include "renderer/Renderer.h"

namespace linguine {

class ButtonSystem : public System {
  public:
    ButtonSystem(EntityManager& entityManager, Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    struct ButtonMetadata {
      uint64_t textEntityId;
    };

    Renderer& _renderer;
};

}  // namespace linguine
