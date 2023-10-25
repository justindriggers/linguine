#include "ModifyHealth.h"

#include <glm/common.hpp>

#include "components/Health.h"
#include "entity/Result.h"

namespace linguine {

void ModifyHealth::execute(Entity& target) {
  auto health = target.get<Health>();
  health->current = glm::clamp(health->current + _power, 0, health->max);
}

}  // namespace linguine
