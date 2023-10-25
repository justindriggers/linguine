#include "ModifyHealthOverTime.h"

#include <glm/common.hpp>

#include "components/EffectTracker.h"
#include "components/Health.h"
#include "entity/Entity.h"

namespace linguine {

void ModifyHealthOverTime::onTick(Component<EffectTracker>& tracker) {
  auto target = _entityManager.getById(tracker->targetId);
  auto health = target->get<Health>();

  health->current = glm::clamp(health->current + _powerPerTick, 0, health->max);
}

}  // namespace linguine
