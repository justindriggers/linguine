#pragma once

#include "System.h"

#include "audio/AudioManager.h"

namespace linguine {

class RiserSystem : public System {
public:
  explicit RiserSystem(EntityManager& entityManager,
                       AudioManager& audioManager)
      : System(entityManager), _audioManager(audioManager) {}

  void update(float deltaTime) override;

  void fixedUpdate(float fixedDeltaTime) override {}

  private:
    AudioManager& _audioManager;
};

}  // namespace linguine
