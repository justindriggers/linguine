#pragma once

#include "System.h"

namespace linguine {

class SelectionDestructionSystem : public System {
  public:
    explicit SelectionDestructionSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
