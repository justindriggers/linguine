#pragma once

#include "System.h"

namespace linguine {

class CastSystem : public System {
  public:
    explicit CastSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
