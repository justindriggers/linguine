#include "CooldownProgressSystem.h"

#include "components/Cooldown.h"
#include "components/Progressable.h"

namespace linguine {

void CooldownProgressSystem::update(float deltaTime) {
  findEntities<Cooldown, Progressable>()->each([deltaTime](const Entity& entity) {
    auto cooldown = entity.get<Cooldown>();
    auto progressable = entity.get<Progressable>();

    progressable->renderable->setEnabled(cooldown->elapsed < cooldown->total);
    progressable->feature->progress = glm::clamp(
            static_cast<float>(cooldown->elapsed) / static_cast<float>(cooldown->total),
            0.0f,
            1.0f
    );

    cooldown->elapsed += deltaTime;
  });
}

}  // namespace linguine
