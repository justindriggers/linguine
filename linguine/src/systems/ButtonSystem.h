#pragma once

#include "System.h"

#include "audio/AudioManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class ButtonSystem : public System {
  public:
    ButtonSystem(EntityManager& entityManager, Renderer& renderer,
                 AudioManager& audioManager)
        : System(entityManager), _renderer(renderer),
          _audioManager(audioManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    struct ButtonMetadata {
      uint64_t textEntityId;
    };

    Renderer& _renderer;
    AudioManager& _audioManager;
};

}  // namespace linguine
