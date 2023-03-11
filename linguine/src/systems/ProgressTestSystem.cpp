#include "ProgressTestSystem.h"

#include "components/Progressable.h"
#include "components/Tapped.h"

namespace linguine {

void ProgressTestSystem::update(float deltaTime) {
  findEntities<Progressable, Tapped>()->each([](const Entity& entity) {
    auto feature = entity.get<Progressable>()->feature;
    feature->progress = glm::clamp(feature->progress + 0.25f, 0.0f, 1.0f);
  });

  findEntities<Progressable>()->each([deltaTime](const Entity& entity) {
    auto feature = entity.get<Progressable>()->feature;
    feature->progress = glm::clamp(feature->progress - deltaTime * 0.25f, 0.0f, 1.0f);
  });
}

}  // namespace linguine
