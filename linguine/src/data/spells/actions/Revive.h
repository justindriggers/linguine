#pragma once

#include "Action.h"

#include <random>

namespace linguine {

class Revive : public Action {
  public:
    explicit Revive(EntityManager& entityManager)
        : _entityManager(entityManager) {}

    ~Revive() override = default;

    void execute(Entity& target) override;

  private:
    std::random_device _random;
    EntityManager& _entityManager;
};

}  // namespace linguine
