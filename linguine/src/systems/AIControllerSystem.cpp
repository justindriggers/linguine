#include "AIControllerSystem.h"

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Cast.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"

namespace linguine {

void AIControllerSystem::update(float deltaTime) {
  findEntities<Cast, Hostile>()->each([this](const Entity& castEntity) {
    auto cast = castEntity.get<Cast>();

    if (!cast->abilityEntityId) {
      findEntities<GlobalCooldown, Hostile>()->each([this, &cast](const Entity& gcdEntity) {
        auto globalCooldown = gcdEntity.get<GlobalCooldown>();

        if (globalCooldown->remaining <= 0.0f) {
          auto availableAbilities = std::vector<Entity>();

          findEntities<Ability, Hostile, Alive>()->each([&availableAbilities](const Entity& abilityEntity) {
            auto ability = abilityEntity.get<Ability>();

            if (ability->remainingCooldown <= 0.0f) {
              availableAbilities.push_back(abilityEntity);
            }
          });

          if (!availableAbilities.empty()) {
            auto shouldBeDefensive = false;

            findEntities<Health, Hostile>()->each([&shouldBeDefensive](const Entity& healthEntity) {
              auto health = healthEntity.get<Health>();

              if (health->current > 0 && static_cast<float>(health->current) / static_cast<float>(health->max) < 0.5f) {
                shouldBeDefensive = true;
              }
            });

            auto defensiveAbilities = std::vector<Entity>();
            auto otherAbilities = std::vector<Entity>();

            for (const auto& abilityEntity : availableAbilities) {
              auto ability = abilityEntity.get<Ability>();

              if (ability->spell.category == Category::Defense) {
                defensiveAbilities.push_back(abilityEntity);
              } else {
                otherAbilities.push_back(abilityEntity);
              }
            }

            std::optional<uint64_t> chosenAbilityId = {};

            if (shouldBeDefensive && !defensiveAbilities.empty()) {
              auto randomAbility = std::uniform_int_distribution<>(0, static_cast<int>(defensiveAbilities.size() - 1));
              const auto index = randomAbility(_random);
              chosenAbilityId = defensiveAbilities[index].getId();
            } else if (!otherAbilities.empty()) {
              auto randomAbility = std::uniform_int_distribution<>(0, static_cast<int>(otherAbilities.size() - 1));
              const auto index = randomAbility(_random);
              chosenAbilityId = otherAbilities[index].getId();
            }

            if (chosenAbilityId) {
              auto abilityEntity = getEntityById(*chosenAbilityId);
              auto ability = abilityEntity->get<Ability>();

              std::optional<uint64_t> chosenTargetId = {};

              switch (ability->spell.category) {
              case Category::Offense:
              case Category::Control: {
                auto availableTargets = findEntities<Health, Friendly, Alive>()->get();

                if (!availableTargets.empty()) {
                  auto randomTarget = std::uniform_int_distribution<>(0, static_cast<int>(availableTargets.size() - 1));
                  const auto index = randomTarget(_random);
                  chosenTargetId = availableTargets[index]->getId();
                }

                break;
              }
              case Category::Defense: {
                auto lowestHp = 1.0f;

                findEntities<Health, Hostile, Alive>()->each([&chosenTargetId, &lowestHp](const Entity& targetEntity) {
                  auto health = targetEntity.get<Health>();
                  auto hp = static_cast<float>(health->current) / static_cast<float>(health->max);

                  if (hp <= lowestHp) {
                    lowestHp = hp;
                    chosenTargetId = targetEntity.getId();
                  }
                });
                break;
              }
              case Category::Support: {
                auto availableTargets = findEntities<Health, Hostile, Alive>()->get();

                if (!availableTargets.empty()) {
                  auto randomTarget = std::uniform_int_distribution<>(0, static_cast<int>(availableTargets.size() - 1));
                  const auto index = randomTarget(_random);
                  chosenTargetId = availableTargets[index]->getId();
                }

                break;
              }
              }

              if (chosenTargetId) {
                cast->abilityEntityId = *chosenAbilityId;
                cast->targetEntityId = *chosenTargetId;
                globalCooldown->remaining = globalCooldown->total;
              }
            }
          }
        }
      });
    }
  });
}

}  // namespace linguine
