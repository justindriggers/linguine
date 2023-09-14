#include "DamageOverTime.h"

#include <glm/common.hpp>

#include "components/EffectTracker.h"
#include "components/Health.h"
#include "entity/Entity.h"

namespace linguine {

void DamageOverTime::onTick(Component<EffectTracker>& tracker) {
  auto target = _entityManager.getById(tracker->targetId);
  auto health = target->get<Health>();
  health->current = glm::max(health->current - _powerPerTick, 0);
}

}  // namespace linguine
