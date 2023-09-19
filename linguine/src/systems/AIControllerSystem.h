#pragma once

#include "System.h"

#include <random>

namespace linguine {

class AIControllerSystem : public System {
  public:
    explicit AIControllerSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    std::random_device _random;
};

}  // namespace linguine
