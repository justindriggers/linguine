#include "Effect.h"

#include "components/EffectTracker.h"
#include "entity/Entity.h"

namespace linguine {

void Effect::update(Component<EffectTracker>& tracker) {
  auto tickDuration = getDuration() / static_cast<float>(getTicks());

  while (tracker->timeSinceLastTick >= tickDuration) {
    if (++tracker->ticks <= getTicks()) {
      onTick(tracker);
    }

    tracker->timeSinceLastTick -= tickDuration;
  }
}

}  // namespace linguine
