#pragma once

#include "System.h"

#include <random>

#include "data/Grid.h"

namespace linguine {

class EnemyAttackSystem : public System {
  public:
    EnemyAttackSystem(EntityManager& entityManager,
                      Grid& grid)
        : System(entityManager), _grid(grid) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Grid& _grid;
    std::random_device _random;
};

}  // namespace linguine
