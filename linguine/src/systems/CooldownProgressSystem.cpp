#include "CooldownProgressSystem.h"

#include "components/Ability.h"
#include "components/GlobalCooldown.h"
#include "components/Progressable.h"
#include "components/TimeWarp.h"

namespace linguine {

void CooldownProgressSystem::update(float deltaTime) {
  float warpedDeltaTime = deltaTime;

  findEntities<TimeWarp>()->each([&warpedDeltaTime](const Entity& entity) {
    auto timeWarp = entity.get<TimeWarp>();
    warpedDeltaTime /= timeWarp->factor;
  });

  findEntities<Ability>()->each([warpedDeltaTime](const Entity& entity) {
    auto ability = entity.get<Ability>();

    if (ability->spell.cooldown > 0.0f) {
      ability->remainingCooldown = glm::max(0.0f, ability->remainingCooldown - warpedDeltaTime);
    }
  });

  findEntities<GlobalCooldown>()->each([warpedDeltaTime](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();
    globalCooldown->remaining = glm::max(0.0f, globalCooldown->remaining - warpedDeltaTime);
  });

  findEntities<GlobalCooldown>()->each([this](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();

    findEntities<Ability, Progressable>()->each([&globalCooldown](const Entity& entity) {
      auto ability = entity.get<Ability>();

      auto progressable = entity.get<Progressable>();
      auto progress = 1.0f - globalCooldown->remaining / globalCooldown->total;

      if (ability->spell.cooldown > 0.0f) {
        if (ability->remainingCooldown > globalCooldown->remaining) {
          progress = 1.0f - ability->remainingCooldown / ability->spell.cooldown;
        }
      }

      progressable->feature->progress = glm::clamp(progress, 0.0f, 1.0f);
    });
  });
}

}  // namespace linguine
