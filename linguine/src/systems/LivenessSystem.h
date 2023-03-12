#pragma once

#include "System.h"

namespace linguine {

class LivenessSystem : public System {
  public:
    explicit LivenessSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
