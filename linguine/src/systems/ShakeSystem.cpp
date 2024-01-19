#include "ShakeSystem.h"

#include "components/Shake.h"
#include "components/Transform.h"

namespace linguine {

void ShakeSystem::update(float deltaTime) {
  if (!_saveManager.isScreenShakeEnabled()) {
    return;
  }

  findEntities<Transform, Shake>()->each([this, deltaTime](const Entity& entity) {
    auto transform = entity.get<Transform>();
    auto shake = entity.get<Shake>();

    if (shake->duration > 0.0f) {
      shake->elapsed += deltaTime;

      if (shake->elapsed >= shake->duration) {
        shake->magnitude = 0.0f;
        shake->elapsed = 0.0f;
        shake->duration = 0.0f;
      } else {
        auto randomOffset = std::uniform_real_distribution(0.0f, shake->magnitude - shake->magnitude / shake->duration * shake->elapsed);
        auto randomDirection = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

        transform->position += glm::angleAxis(randomDirection(_random), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(1.0f, 0.0f, 0.0f) * randomOffset(_random);
      }
    }
  });
}

}  // namespace linguine
