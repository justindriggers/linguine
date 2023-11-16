#pragma once

#include "SaveManager.h"
#include "System.h"

namespace linguine {

class WalletSystem : public System {
  public:
    WalletSystem(EntityManager& entityManager, SaveManager& saveManager)
        : System(entityManager), _saveManager(saveManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SaveManager& _saveManager;
};

}  // namespace linguine
