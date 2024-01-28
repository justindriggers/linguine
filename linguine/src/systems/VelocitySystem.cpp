#include "VelocitySystem.h"

#include <glm/common.hpp>

#include "components/Boost.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/TimeWarp.h"
#include "components/Velocity.h"

namespace linguine {

void VelocitySystem::fixedUpdate(float fixedDeltaTime) {
  auto factor = 1.0f;

  findEntities<TimeWarp>()->each([fixedDeltaTime, &factor](Entity& entity) {
    auto timeWarp = entity.get<TimeWarp>();
    timeWarp->elapsed += fixedDeltaTime;

    if (timeWarp->elapsed >= timeWarp->duration) {
      entity.destroy();
    } else {
      factor *= timeWarp->factor;
    }
  });

  _timeManager.setTimeScale(factor);

  findEntities<Boost>()->each([fixedDeltaTime](Entity& entity) {
    auto boost = entity.get<Boost>();
    boost->elapsed += fixedDeltaTime;

    if (boost->elapsed >= boost->duration) {
      entity.remove<Boost>();
    }
  });

  findEntities<Player, Velocity>()->each([fixedDeltaTime](const Entity& entity) {
    auto player = entity.get<Player>();
    auto velocity = entity.get<Velocity>();

    player->speed = glm::clamp(player->speed + player->acceleration * fixedDeltaTime, 0.0f, player->maxSpeed);
    velocity->velocity.y = player->speed;

    if (entity.has<Boost>()) {
      auto boost = entity.get<Boost>();
      velocity->velocity.y += boost->magnitude * (1.0f - boost->elapsed / boost->duration);
    }
  });

  findEntities<Velocity, PhysicalState>()->each([fixedDeltaTime](const Entity& entity) {
    const auto velocity = entity.get<Velocity>();
    auto physicalState = entity.get<PhysicalState>();
    physicalState->currentPosition += velocity->velocity * fixedDeltaTime;
  });
}

}  // namespace linguine
