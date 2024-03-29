#pragma once

#include "System.h"

namespace linguine {

class CameraFollowSystem : public System {
  public:
    explicit CameraFollowSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;
};

}  // namespace linguine
