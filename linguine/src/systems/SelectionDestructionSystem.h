#pragma once

#include "System.h"

#include "AudioManager.h"

namespace linguine {

class SelectionDestructionSystem : public System {
  public:
    SelectionDestructionSystem(EntityManager& entityManager,
                               AudioManager& audioManager)
        : System(entityManager), _audioManager(audioManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    AudioManager& _audioManager;
};

}  // namespace linguine
