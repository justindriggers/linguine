#include "Apply.h"

#include "components/EffectTracker.h"
#include "components/Progressable.h"
#include "components/Transform.h"
#include "entity/Result.h"

namespace linguine {

void Apply::execute(Entity& target) {
  auto targetId = target.getId();
  bool found = false;

  _entityManager.find<EffectTracker>()->each([this, targetId, &found](const Entity& effectTrackerEntity) {
    auto effectTracker = effectTrackerEntity.get<EffectTracker>();

    if (effectTracker->targetId == targetId && &effectTracker->effect == _effect.get()) {
      effectTracker->ticks = 0;
      effectTracker->timeSinceApplication = effectTracker->timeSinceLastTick;
      found = true;
    }
  });

  if (!found) {
    auto effectEntity = _entityManager.create();

    auto effectTracker = effectEntity->add<EffectTracker>(*_effect, targetId);

    auto transform = effectEntity->add<Transform>();
    transform->position = glm::vec3(0.0f, 0.0f, 1.0f);
    transform->scale = glm::vec3(64.0f, 8.0f, 0.0f);

    auto& renderer = _serviceLocator.get<Renderer>();

    auto progressable = effectEntity->add<Progressable>();
    progressable->feature = new ProgressFeature();
    progressable->feature->color = { 0.0f, 1.0f, 0.0f };
    progressable->feature->meshType = Quad;
    progressable->renderable = renderer.create(std::unique_ptr<RenderFeature>(progressable->feature), UI);
    progressable->renderable->setEnabled(false);
    progressable.setRemovalListener([progressable](const Entity e) {
      progressable->renderable->destroy();
    });

    _effect->onApply(effectTracker);
  }
}

}  // namespace linguine
