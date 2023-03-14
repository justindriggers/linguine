#include "PlayerControllerSystem.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/BigHeal.h"
#include "components/Cooldown.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Tapped.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<GlobalCooldown>()->each([this](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();

    if (globalCooldown->elapsed >= globalCooldown->total) {
      findEntities<Friendly, Alive, Health, Tapped>()->each([globalCooldown](const Entity& entity) {
        auto health = entity.get<Health>();
        health->current = glm::clamp(health->current + 500, 0, health->max);
        globalCooldown->elapsed = 0.0f;
      });

      findEntities<BigHeal, Tapped, Cooldown>()->each([this, globalCooldown](const Entity& entity) {
        auto cooldown = entity.get<Cooldown>();

        if (cooldown->elapsed >= cooldown->total) {
          auto bigHeal = entity.get<BigHeal>();

          findEntities<Friendly, Alive, Health>()->each([bigHeal](const Entity& entity) {
            auto health = entity.get<Health>();
            health->current = glm::clamp(health->current + bigHeal->power, 0, health->max);
          });

          cooldown->elapsed = 0.0f;
          globalCooldown->elapsed = 0.0f;
        }
      });
    }
  });
}

}  // namespace linguine
