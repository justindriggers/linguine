#include "FriendlyAttackSystem.h"

#include <random>

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Unit.h"

namespace linguine {

void FriendlyAttackSystem::update(float deltaTime) {
  auto enemies = findEntities<Hostile, Health, Alive>()->get();

  if (enemies.empty()) {
    return;
  }

  auto random = std::random_device();
  auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(enemies.size() - 1));

  findEntities<Friendly, Unit, Alive>()->each([deltaTime, &enemies, &random, &randomEntity](const Entity& entity) {
    auto unit = entity.get<Unit>();

    if (unit->attackTimer >= unit->attackSpeed) {
      unit->attackTimer -= unit->attackSpeed;

      auto index = randomEntity(random);
      auto target = enemies[index];
      auto health = target->get<Health>();

      health->current = glm::clamp<int32_t>(health->current - unit->attackPower, 0, health->max);
    }

    unit->attackTimer += deltaTime;
  });
}

}  // namespace linguine
