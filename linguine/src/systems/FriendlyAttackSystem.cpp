#include "FriendlyAttackSystem.h"

#include <random>

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Hostile.h"

namespace linguine {

void FriendlyAttackSystem::update(float deltaTime) {
  auto enemies = findEntities<Hostile, Health, Alive>()->get();

  if (enemies.empty()) {
    return;
  }

  auto random = std::random_device();
  auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(enemies.size() - 1));

  findEntities<Friendly, Alive>()->each([deltaTime, &enemies, &random, &randomEntity](const Entity& entity) {
    auto friendly = entity.get<Friendly>();

    if (friendly->attackTimer >= friendly->attackSpeed) {
      friendly->attackTimer -= friendly->attackSpeed;

      auto index = randomEntity(random);
      auto target = enemies[index];
      auto health = target->get<Health>();

      health->current = glm::clamp<int32_t>(health->current - friendly->attackPower, 0, health->max);
    }

    friendly->attackTimer += deltaTime;
  });
}

}  // namespace linguine
