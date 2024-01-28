#include "ShakeSystem.h"

#include "components/CameraFixture.h"
#include "components/Shake.h"
#include "components/Transform.h"

namespace linguine {

void ShakeSystem::update(float deltaTime) {
  if (!_saveManager.isScreenShakeEnabled()) {
    return;
  }

  findEntities<Shake>()->each([deltaTime](Entity& entity) {
    auto shake = entity.get<Shake>();

    if (shake->duration > 0.0f) {
      shake->elapsed += deltaTime;

      if (shake->elapsed >= shake->duration) {
        entity.destroy();
      }
    }
  });

  findEntities<CameraFixture, Transform>()->each([this](const Entity& entity) {
    auto cameraFixture = entity.get<CameraFixture>();

    if (!cameraFixture->shake) {
      return;
    }

    auto magnitude = 0.0f;

    findEntities<Shake>()->each([&magnitude](const Entity& shakeEntity) {
      auto shake = shakeEntity.get<Shake>();
      magnitude += shake->magnitude - shake->magnitude / shake->duration * shake->elapsed;
    });

    auto transform = entity.get<Transform>();

    auto randomOffset = std::uniform_real_distribution(0.0f, magnitude);
    auto randomDirection = std::uniform_real_distribution(0.0f, glm::two_pi<float>());

    transform->position += glm::angleAxis(randomDirection(_random), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(1.0f, 0.0f, 0.0f) * randomOffset(_random);
  });
}

}  // namespace linguine
