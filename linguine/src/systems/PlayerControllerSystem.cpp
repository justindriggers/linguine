#include "PlayerControllerSystem.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Cooldown.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Tapped.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<Cooldown>()->each([this](const Entity& entity) {
    auto cooldown = entity.get<Cooldown>();

    if (cooldown->elapsed >= cooldown->total) {
      findEntities<Friendly, Alive, Health, Tapped>()->each([cooldown](const Entity& entity) {
        auto health = entity.get<Health>();
        health->current = glm::clamp(health->current + 500, 0, health->max);
        cooldown->elapsed = 0.0f;
      });
    }
  });
}

}  // namespace linguine
