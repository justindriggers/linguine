#include "Explosion.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Health.h"

namespace linguine {

void Explosion::execute(Entity& target) {
  auto targets = _entityManager.find<Health, Alive>()->get();
  auto powerPerTarget = _power / static_cast<int>(targets.size());

  for (const auto& entity : targets) {
    auto health = entity->get<Health>();
    health->current = glm::clamp(health->current - powerPerTarget, 0, health->max);
  }
}

}  // namespace linguine
