#pragma once

#include "System.h"

namespace linguine {

class ProgressTestSystem : public System {
  public:
    explicit ProgressTestSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
