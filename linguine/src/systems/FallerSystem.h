#pragma once

#include "System.h"

namespace linguine {

class FallerSystem : public System {
public:
  explicit FallerSystem(EntityManager& entityManager)
      : System(entityManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
