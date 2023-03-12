#include "PlayerControllerSystem.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Tapped.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<Friendly, Alive, Health, Tapped>()->each([](const Entity& entity) {
    auto health = entity.get<Health>();
    health->current = glm::clamp(health->current + 50, 0, health->max);
  });
}

}  // namespace linguine
