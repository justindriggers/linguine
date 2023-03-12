#pragma once

#include "System.h"

namespace linguine {

class EnemyAttackSystem : public System {
  public:
    explicit EnemyAttackSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
