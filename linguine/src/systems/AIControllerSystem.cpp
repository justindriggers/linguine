#include "AIControllerSystem.h"

#include <list>

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Cast.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/UnitType.h"
#include "data/spells/TypeEvaluator.h"

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
            std::list<uint64_t> prioritizedTargets = {};

            findEntities<Health, Alive>()->each([this, &prioritizedTargets](const Entity& targetEntity) {
              if (prioritizedTargets.empty()) {
                prioritizedTargets.push_back(targetEntity.getId());
              } else {
                auto health = targetEntity.get<Health>();
                auto hp = static_cast<float>(health->current) / static_cast<float>(health->max);

                auto inserted = false;

                auto it = prioritizedTargets.begin();

                while (it != prioritizedTargets.end()) {
                  auto other = getEntityById(*it);

                  if (targetEntity.has<Hostile>() && hp <= 0.5f) {
                    auto otherHealth = other->get<Health>();
                    auto lowestHp = static_cast<float>(otherHealth->current) / static_cast<float>(otherHealth->max);

                    if (hp < lowestHp) {
                      prioritizedTargets.insert(it, targetEntity.getId());
                      inserted = true;
                      break;
                    } else if (hp == lowestHp) {
                      auto randomOrder = std::uniform_int_distribution<>(0, 1);

                      if (randomOrder(_random) > 0) {
                        prioritizedTargets.insert(it, targetEntity.getId());
                      } else {
                        prioritizedTargets.insert(std::next(it), targetEntity.getId());
                      }

                      inserted = true;
                      break;
                    }
                  } else {
                    auto randomOrder = std::uniform_int_distribution<>(0, 1);

                    if (randomOrder(_random) > 0) {
                      prioritizedTargets.insert(it, targetEntity.getId());
                    } else {
                      prioritizedTargets.insert(std::next(it), targetEntity.getId());
                    }

                    inserted = true;
                    break;
                  }

                  it = std::next(it);
                }

                if (!inserted) {
                  prioritizedTargets.push_back(targetEntity.getId());
                }
              }
            });

            std::optional<uint64_t> chosenTargetId = {};
            std::optional<uint64_t> chosenAbilityId = {};

            for (const auto& targetId : prioritizedTargets) {
              auto target = getEntityById(targetId);
              auto targetType = target->get<UnitType>()->type;

              for (const auto& abilityEntity : availableAbilities) {
                auto ability = abilityEntity.get<Ability>();

                if (target->has<Hostile>()) {
                  if (TypeEvaluator::calculateModifier(ability->spell.type, targetType) > 0.0f) {
                    chosenTargetId = targetId;
                    chosenAbilityId = abilityEntity.getId();
                    break;
                  }
                } else if (target->has<Friendly>()) {
                  if (TypeEvaluator::calculateModifier(ability->spell.type, targetType) < 0.0f) {
                    chosenTargetId = targetId;
                    chosenAbilityId = abilityEntity.getId();
                    break;
                  }
                }
              }

              if (chosenTargetId && chosenAbilityId) {
                cast->abilityEntityId = *chosenAbilityId;
                cast->targetEntityId = *chosenTargetId;
                globalCooldown->remaining = globalCooldown->total;
                break;
              }
            }
          }
        }
      });
    }
  });
}

}  // namespace linguine
