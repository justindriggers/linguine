#include "EnemyAttackSystem.h"

#include <random>

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Hostile.h"

namespace linguine {

void EnemyAttackSystem::update(float deltaTime) {
  auto friendlies = findEntities<Friendly, Health, Alive>()->get();

  if (friendlies.empty()) {
    return;
  }

  auto random = std::random_device();
  auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(friendlies.size() - 1));

  findEntities<Hostile, Alive>()->each([deltaTime, &friendlies, &random, &randomEntity](const Entity& entity) {
    auto hostile = entity.get<Hostile>();

    if (hostile->attackTimer >= hostile->attackSpeed) {
      hostile->attackTimer -= hostile->attackSpeed;

      auto index = randomEntity(random);
      auto target = friendlies[index];
      auto health = target->get<Health>();

      health->current = glm::clamp<int32_t>(health->current - hostile->attackPower, 0, health->max);
    }

    hostile->attackTimer += deltaTime;
  });
}

}  // namespace linguine
