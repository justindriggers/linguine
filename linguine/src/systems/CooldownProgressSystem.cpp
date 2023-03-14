#include "CooldownProgressSystem.h"

#include "components/Cooldown.h"
#include "components/GlobalCooldown.h"
#include "components/Progressable.h"

namespace linguine {

void CooldownProgressSystem::update(float deltaTime) {
  findEntities<Cooldown, Progressable>()->each([deltaTime](const Entity& entity) {
    auto cooldown = entity.get<Cooldown>();
    auto progressable = entity.get<Progressable>();

    progressable->feature->progress = glm::clamp(
            static_cast<float>(cooldown->elapsed) / static_cast<float>(cooldown->total),
            0.0f,
            1.0f
    );

    cooldown->elapsed += deltaTime;
  });

  findEntities<GlobalCooldown, Progressable>()->each([deltaTime](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();
    auto progressable = entity.get<Progressable>();

    progressable->renderable->setEnabled(globalCooldown->elapsed < globalCooldown->total);
    progressable->feature->progress = glm::clamp(
        static_cast<float>(globalCooldown->elapsed) / static_cast<float>(globalCooldown->total),
        0.0f,
        1.0f
    );

    globalCooldown->elapsed += deltaTime;
  });
}

}  // namespace linguine
