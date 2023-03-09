#pragma once

#include "System.h"

#include "audio/AudioManager.h"

namespace linguine {

class RotatorSystem : public System {
  public:
    explicit RotatorSystem(EntityManager& entityManager,
                           AudioManager& audioManager)
        : System(entityManager), _audioManager(audioManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    AudioManager& _audioManager;
};

}  // namespace linguine
