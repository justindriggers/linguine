#pragma once

#include "System.h"

namespace linguine {

class ProjectileSystem : public System {
  public:
    explicit ProjectileSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;
};

}  // namespace linguine
