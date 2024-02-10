#pragma once

#include "System.h"

namespace linguine {

class CollapseSystem : public System {
  public:
    explicit CollapseSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
