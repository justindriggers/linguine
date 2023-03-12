#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/Dead.h"
#include "components/Health.h"

namespace linguine {

void LivenessSystem::update(float deltaTime) {
  findEntities<Alive, Health>()->each([](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current <= 0) {
      entity.remove<Alive>();
      entity.add<Dead>();
    }
  });

  findEntities<Dead, Health>()->each([](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current > 0) {
      entity.remove<Dead>();
      entity.add<Alive>();
    }
  });
}

}  // namespace linguine
