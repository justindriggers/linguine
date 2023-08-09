#include "Heal.h"

#include <glm/common.hpp>

#include "entity/Result.h"
#include "components/Health.h"

namespace linguine {

void Heal::execute(Entity& target) {
  auto health = target.get<Health>();
  health->current = glm::min(health->current + _power, health->max);
}

}  // namespace linguine
