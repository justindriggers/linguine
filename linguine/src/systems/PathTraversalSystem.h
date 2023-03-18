#pragma once

#include "System.h"

namespace linguine {

class PathTraversalSystem : public System {
  public:
    explicit PathTraversalSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
