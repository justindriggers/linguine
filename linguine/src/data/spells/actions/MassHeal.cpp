#include "MassHeal.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Health.h"

namespace linguine {

void MassHeal::execute(Entity& target) {
  _entityManager.find<Health, Alive>()->each([this](const Entity& entity) {
    auto health = entity.get<Health>();
    health->current = glm::clamp(health->current + static_cast<int32_t>(glm::round(static_cast<float>(health->max) * _power)), 0, health->max);
  });
}

}  // namespace linguine
