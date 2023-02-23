#pragma once

#include "System.h"

namespace linguine {

class RiserSystem : public System {
public:
  explicit RiserSystem(EntityManager& entityManager)
      : System(entityManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
