#pragma once

#include "System.h"

#include <random>

#include "ServiceLocator.h"

namespace linguine {

class LivenessSystem : public System {
  public:
    LivenessSystem(EntityManager& entityManager,
                   Renderer& renderer,
                   SaveManager& saveManager,
                   ServiceLocator& serviceLocator)
        : System(entityManager), _renderer(renderer),
          _saveManager(saveManager),
          _serviceLocator(serviceLocator) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    std::random_device _random;

    Renderer& _renderer;
    SaveManager& _saveManager;
    ServiceLocator& _serviceLocator;
};

}  // namespace linguine
