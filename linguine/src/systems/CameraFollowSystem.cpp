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
      cameraState->currentPosition += ((playerState->currentPosition - cameraState->currentPosition) - glm::vec2(0.0f, 3.0f)) * 0.1f;
    });
  });
}

}  // namespace linguine
