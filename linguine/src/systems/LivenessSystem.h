#pragma once

#include "System.h"

#include "ServiceLocator.h"

namespace linguine {

class LivenessSystem : public System {
  public:
    LivenessSystem(EntityManager& entityManager,
                   SaveManager& saveManager,
                   ServiceLocator& serviceLocator)
        : System(entityManager), _saveManager(saveManager),
          _serviceLocator(serviceLocator) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SaveManager& _saveManager;
    ServiceLocator& _serviceLocator;
};

}  // namespace linguine
