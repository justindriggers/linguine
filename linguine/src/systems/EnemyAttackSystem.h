#pragma once

#include "System.h"

#include <random>

#include "data/Grid.h"
#include "renderer/Renderer.h"

namespace linguine {

class EnemyAttackSystem : public System {
  public:
    EnemyAttackSystem(EntityManager& entityManager,
                      Renderer& renderer,
                      Grid& grid)
        : System(entityManager), _renderer(renderer), _grid(grid) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    Renderer& _renderer;
    Grid& _grid;
    std::random_device _random;
};

}  // namespace linguine
