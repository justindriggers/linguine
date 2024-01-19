#pragma once

#include "System.h"

#include <random>

#include "SaveManager.h"

namespace linguine {

class ShakeSystem : public System {
  public:
    ShakeSystem(EntityManager& entityManager,
                SaveManager& saveManager)
        : System(entityManager),
          _saveManager(saveManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    std::random_device _random;

    SaveManager& _saveManager;
};

}  // namespace linguine
