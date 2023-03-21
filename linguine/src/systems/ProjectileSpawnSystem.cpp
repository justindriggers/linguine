#include "ProjectileSpawnSystem.h"

#include <glm/gtc/quaternion.hpp>

#include "components/PhysicalState.h"
#include "components/SpawnPoint.h"

namespace linguine {

void ProjectileSpawnSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<SpawnPoint, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto spawnPoint = entity.get<SpawnPoint>();
    auto physicalState = entity.get<PhysicalState>();

    spawnPoint->elapsed += fixedDeltaTime;

    while (spawnPoint->elapsed >= spawnPoint->interval) {
      spawnPoint->elapsed -= spawnPoint->interval;

      auto direction = glm::angleAxis(physicalState->currentRotation, glm::vec3(0.0f, 0.0f, 1.0f))
                       * glm::vec3(0.0f, 1.0f, 0.0f);

      for (auto i = 0; i < spawnPoint->count; ++i) {
        auto angle = glm::two_pi<float>() / static_cast<float>(spawnPoint->count) * static_cast<float>(i);
        auto newDirection = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f)) * direction;
        _projectileFactory.create(physicalState->currentPosition, glm::vec2(newDirection) * 1.0f, 20);
      }
    }
  });
}

}  // namespace linguine
