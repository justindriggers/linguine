#pragma once

#include "Action.h"

#include "ServiceLocator.h"
#include "data/spells/effects/Effect.h"
#include "entity/EntityManager.h"

namespace linguine {

class Apply : public Action {
  public:
    Apply(ServiceLocator& serviceLocator, EntityManager& entityManager,
          std::unique_ptr<Effect> effect)
        : _effect(std::move(effect)), _serviceLocator(serviceLocator),
          _entityManager(entityManager) {}

    ~Apply() override = default;

    void execute(Entity& target) override;

  private:
    ServiceLocator& _serviceLocator;
    EntityManager& _entityManager;
    std::unique_ptr<Effect> _effect;
};

}  // namespace linguine
