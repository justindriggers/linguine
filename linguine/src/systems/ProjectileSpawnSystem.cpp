#include "ProjectileSpawnSystem.h"

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

      _projectileFactory.create(physicalState->currentPosition, 1.0f, 20);
    }
  });
}

}  // namespace linguine
