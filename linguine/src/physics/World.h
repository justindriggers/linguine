#pragma once

#include "QuadTree.h"
#include "entity/EntityManager.h"

namespace linguine::physics {

class World {
  public:
    explicit World(EntityManager& entityManager);

    ~World();

    void add(Entity& entity);

    void remove(const Entity& entity);

    void update();

    void applyToCandidatePairs(const std::function<void(Entity& a, Entity& b)>& function);

  private:
    EntityManager& _entityManager;
    QuadTree* _root;
};

}  // namespace linguine::physics
