#pragma once

#include "System.h"

#include <random>

namespace linguine {

class FireSystem : public System {
  public:
    explicit FireSystem(EntityManager& entityManager) : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    std::random_device _random;
};

}  // namespace linguine
