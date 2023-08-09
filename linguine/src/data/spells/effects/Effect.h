#pragma once

#include "entity/Component.h"

namespace linguine {

struct EffectTracker;

class Effect {
  public:
    virtual ~Effect() = default;

    [[nodiscard]] float getDuration() const {
      return _duration;
    }

    [[nodiscard]] uint32_t getTicks() const {
      return _ticks;
    }

    void update(Component<EffectTracker>& tracker);

    virtual void onApply(Component<EffectTracker>& tracker) = 0;

    virtual void onTick(Component<EffectTracker>& tracker) = 0;

    virtual void onRemove(Component<EffectTracker>& tracker) = 0;

  protected:
    Effect(float duration, uint32_t ticks)
        : _duration(duration), _ticks(ticks) {}

  private:
    float _duration;
    uint32_t _ticks;
};

}  // namespace linguine
