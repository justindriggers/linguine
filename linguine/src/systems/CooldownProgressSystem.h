#pragma once

#include "System.h"

namespace linguine {

class CooldownProgressSystem : public System {
  public:
    explicit CooldownProgressSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
