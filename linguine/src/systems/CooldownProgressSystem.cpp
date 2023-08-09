#include "CooldownProgressSystem.h"

#include "components/Ability.h"
#include "components/GlobalCooldown.h"
#include "components/Progressable.h"

namespace linguine {

void CooldownProgressSystem::update(float deltaTime) {
  findEntities<GlobalCooldown>()->each([this, deltaTime](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();
    globalCooldown->elapsed += deltaTime;

    findEntities<Ability, Progressable>()->each([deltaTime, &globalCooldown](const Entity& entity) {
      auto ability = entity.get<Ability>();

      auto progressable = entity.get<Progressable>();
      auto progress = globalCooldown->elapsed / globalCooldown->total;

      if (ability->spell.cooldown > 0.0f) {
        ability->remainingCooldown -= deltaTime;

        if (ability->remainingCooldown > globalCooldown->total - globalCooldown->elapsed) {
          progress = 1.0f - ability->remainingCooldown / ability->spell.cooldown;
        }
      }

      progressable->feature->progress = glm::clamp(progress, 0.0f, 1.0f);
    });
  });
}

}  // namespace linguine
