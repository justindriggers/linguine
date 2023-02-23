#pragma once

#include <memory>

#include "EntityManager.h"

namespace linguine {

class EntityManagerFactory {
  public:
    virtual ~EntityManagerFactory() = default;

    virtual std::unique_ptr<EntityManager> create() = 0;
};

}  // namespace linguine
