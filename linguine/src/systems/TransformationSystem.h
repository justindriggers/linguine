#pragma once

#include "System.h"

namespace linguine {

class TransformationSystem : public System {
public:
  explicit TransformationSystem(EntityManager& entityManager)
      : System(entityManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}
};

}  // namespace linguine
