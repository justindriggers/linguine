#include "ModifyHealth.h"

#include <glm/common.hpp>

#include "components/Health.h"
#include "components/UnitType.h"
#include "data/spells/TypeEvaluator.h"
#include "entity/Result.h"

namespace linguine {

void ModifyHealth::execute(Entity& target) {
  auto health = target.get<Health>();
  auto targetType = target.get<UnitType>();

  auto modifier = static_cast<int32_t>(
      glm::round(static_cast<float>(_power) * TypeEvaluator::calculateModifier(_type, targetType->type))
  );

  health->current = glm::min(health->current + modifier, health->max);
}

}  // namespace linguine
