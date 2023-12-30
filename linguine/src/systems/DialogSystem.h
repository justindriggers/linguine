#pragma once

#include "System.h"

namespace linguine {

class DialogSystem : public System {
  public:
    explicit DialogSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
