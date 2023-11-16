#pragma once

#include "System.h"

namespace linguine {

class WalletSystem : public System {
  public:
    explicit WalletSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
