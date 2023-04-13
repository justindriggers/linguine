#include "EnemyAttackSystem.h"

#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/Orbiter.h"
#include "components/Targeting.h"

namespace linguine {

void EnemyAttackSystem::update(float deltaTime) {
  findEntities<Hostile, MeleeAttack, Targeting, GridPosition>()->each([this, deltaTime](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto meleeAttack = entity.get<MeleeAttack>();
    meleeAttack->elapsed += deltaTime;

    if (targeting->current) {
      auto target = getEntityById(*targeting->current);

      if (target->has<GridPosition>()) {
        if (_grid.isAdjacent(target->get<GridPosition>()->position, entity.get<GridPosition>()->position)
            && meleeAttack->elapsed >= meleeAttack->speed) {
          meleeAttack->elapsed = 0.0f;

          auto orbiters = findEntities<Orbiter, Health>()->get();

          if (orbiters.empty()) {
            auto health = target->get<Health>();
            health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
          } else {
            auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(orbiters.size() - 1));
            const auto targetIndex = randomEntity(_random);
            auto health = orbiters[targetIndex]->get<Health>();
            health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
          }
        }
      }
    }
  });
}

}  // namespace linguine
