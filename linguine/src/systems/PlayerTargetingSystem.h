#pragma once

#include "System.h"

namespace linguine {

class PlayerTargetingSystem : public System {
  public:
    explicit PlayerTargetingSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
