#include "ModifyHealthOverTime.h"

#include <glm/common.hpp>

#include "components/EffectTracker.h"
#include "components/Health.h"
#include "components/UnitType.h"
#include "data/spells/TypeEvaluator.h"
#include "entity/Entity.h"

namespace linguine {

void ModifyHealthOverTime::onTick(Component<EffectTracker>& tracker) {
  auto target = _entityManager.getById(tracker->targetId);
  auto health = target->get<Health>();
  auto targetType = target->get<UnitType>();

  auto modifier = static_cast<int32_t>(
      glm::round(static_cast<float>(_powerPerTick) * TypeEvaluator::calculateModifier(getType(), targetType->type))
  );

  health->current = glm::min(health->current + modifier, health->max);
}

}  // namespace linguine
