#pragma once

#include "System.h"

namespace linguine {

class ToggleSystem : public System {
  public:
    explicit ToggleSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    struct ToggleMetadata {
      uint64_t onButtonEntityId;
      uint64_t offButtonEntityId;
    };
};

}  // namespace linguine
