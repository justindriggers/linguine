#pragma once

#include "System.h"

namespace linguine {

class EventSystem : public System {
  public:
    explicit EventSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;
};

}  // namespace linguine
