#pragma once

#include <array>
#include <unordered_set>

#include "components/BoundingBox.h"
#include "entity/Entity.h"

namespace linguine::physics {

class World;

class QuadTree {
  public:
    QuadTree(World& world, EntityManager& entityManager, BoundingBox bounds)
        : _world(world), _entityManager(entityManager), _bounds(bounds) {}

    [[nodiscard]] bool insert(Entity& entity);

    void remove(const Entity& entity);

    [[nodiscard]] bool isEmpty();

    void update();

    void applyToCandidatePairs(const std::function<void(Entity& a, Entity& b)>& function);

  private:
    World& _world;
    EntityManager& _entityManager;
    BoundingBox _bounds;

    std::array<std::unique_ptr<QuadTree>, 4> _nodes = {};
    std::unordered_set<uint64_t> _children;

    void applyTo(const std::function<void(Entity& a, Entity& b)>& function,
                 Entity& a);

    friend class World;
};

}  // namespace linguine::physics
