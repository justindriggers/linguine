#pragma once

#include "Effect.h"

namespace linguine {

class ModifyHealthOverTime : public Effect {
  public:
    ModifyHealthOverTime(EntityManager& entityManager, glm::vec3 color,
                         float duration, uint32_t ticks, int32_t powerPerTick)
        : Effect(color, duration, ticks), _entityManager(entityManager),
          _powerPerTick(powerPerTick) {}

    ~ModifyHealthOverTime() override = default;

    void onApply(Component<EffectTracker>& tracker) override {}

    void onTick(Component<EffectTracker>& tracker) override;

    void onRemove(Component<EffectTracker>& tracker) override {}

  private:
    EntityManager& _entityManager;
    int32_t _powerPerTick;
};

}  // namespace linguine
