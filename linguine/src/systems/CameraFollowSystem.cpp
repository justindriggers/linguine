#include "CameraFollowSystem.h"

#include "components/CameraFixture.h"
#include "components/PhysicalState.h"
#include "components/Player.h"

namespace linguine {

void CameraFollowSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Player, PhysicalState>()->each([this](const Entity& playerEntity) {
    auto playerState = playerEntity.get<PhysicalState>();

    findEntities<CameraFixture, PhysicalState>()->each([playerState](const Entity& cameraEntity) {
      auto cameraState = cameraEntity.get<PhysicalState>();
      cameraState->currentPosition += (playerState->currentPosition - cameraState->currentPosition) * 0.1f;
    });
  });
}

}  // namespace linguine
