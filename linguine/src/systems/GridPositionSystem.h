#pragma once

#include "System.h"

#include "data/Grid.h"

namespace linguine {

class GridPositionSystem : public System {
  public:
    GridPositionSystem(EntityManager& entityManager, Grid& grid)
        : System(entityManager), _grid(grid) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Grid& _grid;
};

}  // namespace linguine
