#include "DirectionalMovementSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/PhysicalState.h"
#include "components/Player.h"

namespace linguine {

void DirectionalMovementSystem::fixedUpdate(float fixedDeltaTime) {
  auto direction = glm::vec2(0.0f);

  if (_inputManager.isKeyPressed(InputManager::W)) {
    direction += glm::vec2(0.0f, 1.0f);
  }

  if (_inputManager.isKeyPressed(InputManager::A)) {
    direction += glm::vec2(-1.0f, 0.0f);
  }

  if (_inputManager.isKeyPressed(InputManager::S)) {
    direction += glm::vec2(0.0f, -1.0f);
  }

  if (_inputManager.isKeyPressed(InputManager::D)) {
    direction += glm::vec2(1.0f, 0.0f);
  }

  if (glm::length2(direction) > 0.0f) {
    direction = glm::normalize(direction);

    findEntities<Player, PhysicalState>()->each([&direction, fixedDeltaTime](Entity &entity) {
      auto physicalState = entity.get<PhysicalState>();
      physicalState->currentPosition += direction * 5.0f * fixedDeltaTime;
      physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
    });
  }
}

}  // namespace linguine
