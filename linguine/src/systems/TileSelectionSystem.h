#pragma once

#include "System.h"

#include "data/Grid.h"
#include "renderer/Renderer.h"

namespace linguine {

class TileSelectionSystem : public System {
  public:
    TileSelectionSystem(EntityManager& entityManager,
                        Renderer& renderer,
                        Grid& grid)
        : System(entityManager), _renderer(renderer), _grid(grid) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;
    Grid& _grid;
};

}  // namespace linguine
