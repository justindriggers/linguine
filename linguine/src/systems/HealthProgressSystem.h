#pragma once

#include "System.h"

namespace linguine {

class HealthProgressSystem : public System {
  public:
    explicit HealthProgressSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
