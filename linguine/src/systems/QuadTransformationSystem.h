#pragma once

#include "System.h"

namespace linguine {

class QuadTransformationSystem : public System {
public:
  explicit QuadTransformationSystem(EntityManager& entityManager)
      : System(entityManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
