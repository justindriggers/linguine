#pragma once

#include "System.h"

namespace linguine {

class AttachmentSystem : public System {
  public:
    explicit AttachmentSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;
};

}  // namespace linguine
