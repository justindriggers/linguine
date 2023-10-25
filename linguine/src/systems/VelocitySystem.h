#pragma once

#include "System.h"

namespace linguine {

class VelocitySystem : public System {
  public:
    explicit VelocitySystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;
};

}  // namespace linguine
