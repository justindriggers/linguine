#pragma once

#include "System.h"

namespace linguine {

class TutorialSystem : public System {
  public:
    explicit TutorialSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
