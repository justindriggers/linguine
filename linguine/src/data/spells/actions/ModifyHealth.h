#pragma once

#include "Action.h"

#include "entity/EntityManager.h"

namespace linguine {

class ModifyHealth : public Action {
  public:
    ModifyHealth(EntityManager& entityManager, int32_t power)
        : _entityManager(entityManager), _power(power) {}

    ~ModifyHealth() override = default;

    void execute(Entity& target) override;

  private:
    EntityManager& _entityManager;
    int32_t _power;
};

}  // namespace linguine
