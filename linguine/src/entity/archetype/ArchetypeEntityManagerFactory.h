#pragma once

#include "entity/EntityManagerFactory.h"

#include "ArchetypeEntityManager.h"

namespace linguine::archetype {

class ArchetypeEntityManagerFactory : public EntityManagerFactory {
  public:
    std::unique_ptr<EntityManager> create() override {
      return std::make_unique<ArchetypeEntityManager>();
    }
};

}  // namespace linguine::archetype
