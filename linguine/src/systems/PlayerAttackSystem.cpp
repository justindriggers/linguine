#include "PlayerAttackSystem.h"

#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/MeleeAttack.h"
#include "components/Player.h"
#include "components/Targeting.h"

namespace linguine {

void PlayerAttackSystem::update(float deltaTime) {
  findEntities<Player, MeleeAttack, Targeting, GridPosition>()->each([this, deltaTime](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto meleeAttack = entity.get<MeleeAttack>();
    meleeAttack->elapsed += deltaTime;

    if (targeting->current) {
      auto target = getEntityById(*targeting->current);

      if (target->has<GridPosition>()) {
        if (_grid.isAdjacent(target->get<GridPosition>()->position, entity.get<GridPosition>()->position)
            && meleeAttack->elapsed >= meleeAttack->speed) {
          meleeAttack->elapsed = 0.0f;

          auto health = target->get<Health>();
          health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
        }
      }
    }
  });
}

}  // namespace linguine
