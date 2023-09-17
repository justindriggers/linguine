#include "CooldownProgressSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Progressable.h"

namespace linguine {

void CooldownProgressSystem::update(float deltaTime) {
  findEntities<Ability>()->each([deltaTime](const Entity& entity) {
    auto ability = entity.get<Ability>();

    if (ability->spell.cooldown > 0.0f) {
      ability->remainingCooldown = glm::max(0.0f, ability->remainingCooldown - deltaTime);
    }
  });

  findEntities<GlobalCooldown>()->each([this, deltaTime](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();
    globalCooldown->remaining = glm::max(0.0f, globalCooldown->remaining - deltaTime);

    findEntities<Friendly, AbilityButton, Progressable>()->each([this, &globalCooldown](const Entity& entity) {
      auto abilityButton = entity.get<AbilityButton>();
      auto abilityEntity = getEntityById(abilityButton->abilityEntityId);
      auto ability = abilityEntity->get<Ability>();

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
