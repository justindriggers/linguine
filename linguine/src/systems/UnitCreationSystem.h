#pragma once

#include "System.h"

namespace linguine {

class UnitCreationSystem : public System {
  public:
    explicit UnitCreationSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
