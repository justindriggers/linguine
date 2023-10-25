#include "VelocitySystem.h"

#include <glm/common.hpp>

#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/Velocity.h"

namespace linguine {

void VelocitySystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Player, Velocity>()->each([fixedDeltaTime](const Entity& entity) {
    auto player = entity.get<Player>();
    auto velocity = entity.get<Velocity>();

    player->speed = glm::clamp(player->speed + player->acceleration * fixedDeltaTime, 0.0f, player->maxSpeed);
    velocity->velocity.y = player->speed;
  });

  findEntities<Velocity, PhysicalState>()->each([fixedDeltaTime](const Entity& entity) {
    const auto velocity = entity.get<Velocity>();
    auto physicalState = entity.get<PhysicalState>();
    physicalState->currentPosition += velocity->velocity * fixedDeltaTime;
  });
}

}  // namespace linguine
