#include "HealthProgressSystem.h"

#include "components/Health.h"
#include "components/Progressable.h"

namespace linguine {

void HealthProgressSystem::update(float deltaTime) {
  findEntities<Health, Progressable>()->each([](const Entity& entity) {
    auto health = entity.get<Health>();
    auto feature = entity.get<Progressable>()->feature;

    feature->progress = glm::clamp(
        static_cast<float>(health->current) / static_cast<float>(health->max),
        0.0f,
        1.0f
    );
  });
}

}  // namespace linguine
