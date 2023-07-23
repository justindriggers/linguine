#pragma once

#include "System.h"

#include <random>

#include "data/Grid.h"
#include "renderer/Renderer.h"

namespace linguine {

class EnemyAttackSystem : public System {
  public:
    EnemyAttackSystem(EntityManager& entityManager,
                      Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Renderer& _renderer;
    std::random_device _random;
};

}  // namespace linguine
