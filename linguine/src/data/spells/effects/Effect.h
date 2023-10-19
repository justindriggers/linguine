#pragma once

#include <glm/vec3.hpp>

#include "data/spells/Type.h"
#include "entity/Component.h"

namespace linguine {

struct EffectTracker;

class Effect {
  public:
    virtual ~Effect() = default;

    [[nodiscard]] Type getType() const {
      return _type;
    }

    [[nodiscard]] glm::vec3 getColor() const {
      return _color;
    }

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
    Effect(Type type, glm::vec3 color, float duration, uint32_t ticks)
        : _type(type), _color(color), _duration(duration), _ticks(ticks) {}

  private:
    Type _type;
    glm::vec3 _color;
    float _duration;
    uint32_t _ticks;
};

}  // namespace linguine
