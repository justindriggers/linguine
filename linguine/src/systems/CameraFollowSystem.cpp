#include "CameraFollowSystem.h"

#include "components/CameraFixture.h"
#include "components/Follow.h"
#include "components/PhysicalState.h"
#include "components/Player.h"

namespace linguine {

void CameraFollowSystem::fixedUpdate(float fixedDeltaTime) {
    findEntities<CameraFixture, PhysicalState, Follow>()->each([this](const Entity& cameraEntity) {
      auto cameraState = cameraEntity.get<PhysicalState>();
      auto follow = cameraEntity.get<Follow>();

      findEntities<Player, PhysicalState>()->each([cameraState, follow](const Entity& playerEntity) {
        auto playerState = playerEntity.get<PhysicalState>();

        cameraState->previousPosition.x = follow->offset.x;
        cameraState->currentPosition.x = follow->offset.x;

        cameraState->previousPosition.y = playerState->previousPosition.y + follow->offset.y;
        cameraState->currentPosition.y = playerState->currentPosition.y + follow->offset.y;
    });
  });
}

}  // namespace linguine
