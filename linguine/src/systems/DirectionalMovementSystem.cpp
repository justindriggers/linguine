#include "DirectionalMovementSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Velocity.h"

namespace linguine {

void DirectionalMovementSystem::update(float deltaTime) {
  auto direction = glm::vec2(0.0f);

  if (_inputManager.isKeyPressed(Key::W)) {
    direction += glm::vec2(0.0f, 1.0f);
  }

  if (_inputManager.isKeyPressed(Key::A)) {
    direction += glm::vec2(-1.0f, 0.0f);
  }

  if (_inputManager.isKeyPressed(Key::S)) {
    direction += glm::vec2(0.0f, -1.0f);
  }

  if (_inputManager.isKeyPressed(Key::D)) {
    direction += glm::vec2(1.0f, 0.0f);
  }

  findEntities<Player, Velocity>()->each([&direction](const Entity& entity) {
    auto velocity = entity.get<Velocity>();

    if (glm::length2(direction) > 0.0f) {
      velocity->velocity = glm::normalize(direction) * 5.0f;
    } else {
      velocity->velocity = { 0.0f, 0.0f };
    }
  });
}

void DirectionalMovementSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Player, Velocity, PhysicalState>()->each([fixedDeltaTime](const Entity &entity) {
    auto velocity = entity.get<Velocity>();
    auto physicalState = entity.get<PhysicalState>();

    physicalState->currentPosition += velocity->velocity * fixedDeltaTime;

    if (glm::length2(velocity->velocity) > 0.0f) {
      auto direction = glm::normalize(velocity->velocity);
      physicalState->currentRotation = glm::atan(direction.y, direction.x) - glm::half_pi<float>();
    }
  });
}

}  // namespace linguine
