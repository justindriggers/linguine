#pragma once

#include "System.h"

namespace linguine {

class RotatorSystem : public System {
  public:
    explicit RotatorSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
