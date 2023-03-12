#pragma once

#include "System.h"

namespace linguine {

class FriendlyAttackSystem : public System {
  public:
    explicit FriendlyAttackSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
