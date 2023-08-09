#pragma once

#include "data/spells/effects/Effect.h"

namespace linguine {

struct EffectTracker {
  EffectTracker(Effect& effect, uint64_t targetId)
      : effect(effect), targetId(targetId) {}

  Effect& effect;
  uint64_t targetId;
  uint32_t ticks = 0;
  float timeSinceLastTick = 0.0f;
  float timeSinceApplication = 0.0f;
};

}  // namespace linguine
