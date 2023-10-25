#pragma once

#include "Action.h"

#include "entity/EntityManager.h"

namespace linguine {

class MassHeal : public Action {
  public:
    MassHeal(EntityManager& entityManager, int32_t power)
        : _entityManager(entityManager), _power(power) {}

    ~MassHeal() override = default;

    void execute(Entity& target) override;

  private:
    EntityManager& _entityManager;
    int32_t _power;
};

}  // namespace linguine
