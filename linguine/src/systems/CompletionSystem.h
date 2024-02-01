#pragma once

#include "System.h"

#include "ServiceLocator.h"

namespace linguine {

class CompletionSystem : public System {
  public:
    CompletionSystem(EntityManager& entityManager,
                     ServiceLocator& serviceLocator)
        : System(entityManager),
          _serviceLocator(serviceLocator) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    ServiceLocator& _serviceLocator;
};

}  // namespace linguine
