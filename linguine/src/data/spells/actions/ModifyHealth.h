#pragma once

#include "Action.h"

#include "data/spells/Type.h"
#include "entity/EntityManager.h"

namespace linguine {

class ModifyHealth : public Action {
  public:
    ModifyHealth(EntityManager& entityManager, Type type, int32_t power)
        : _entityManager(entityManager), _type(type), _power(power) {}

    ~ModifyHealth() override = default;

    void execute(Entity& target) override;

  private:
    EntityManager& _entityManager;
    Type _type;
    int32_t _power;
};

}  // namespace linguine
