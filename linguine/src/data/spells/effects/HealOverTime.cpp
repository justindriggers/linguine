#include "HealOverTime.h"

#include <glm/common.hpp>

#include "components/EffectTracker.h"
#include "components/Health.h"
#include "entity/Entity.h"

namespace linguine {

void HealOverTime::onTick(Component<EffectTracker>& tracker) {
  auto target = _entityManager.getById(tracker->targetId);
  auto health = target->get<Health>();
  health->current = glm::min(health->current + _powerPerTick, health->max);
}

}  // namespace linguine
