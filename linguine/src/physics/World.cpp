#include "World.h"

#include "components/WorldNode.h"
#include "entity/Entity.h"

namespace linguine::physics {

World::World(EntityManager& entityManager)
    : _entityManager(entityManager),
      _root(new QuadTree(*this, entityManager, { 1.0f, -1.0f, -1.0f, 1.0f })) {}

World::~World() {
  delete _root;
}

void World::add(Entity& entity) {
  if (!entity.has<WorldNode>()) {
    entity.add<WorldNode>();
  }

  auto boundingBox = entity.get<BoundingBox>();

  auto width = boundingBox->right - boundingBox->left;
  auto height = boundingBox->top - boundingBox->bottom;

  auto x = boundingBox->left + width / 2.0f;
  auto y = boundingBox->bottom + height / 2.0f;

  while (!_root->insert(entity)) {
    auto oldRoot = _root;
    _root = new QuadTree(*this, _entityManager, {});

    auto rootWidth = oldRoot->_bounds.right - oldRoot->_bounds.left;
    auto rootHeight = oldRoot->_bounds.top - oldRoot->_bounds.bottom;

    auto rootX = oldRoot->_bounds.left + rootWidth / 2.0f;
    auto rootY = oldRoot->_bounds.bottom + rootHeight / 2.0f;

    auto bounds = oldRoot->_bounds;

    if (x <= rootX && y >= rootY) { // Expand Bottom-Right
      bounds.top += rootHeight;
      bounds.left -= rootWidth;
      _root->_nodes[3] = std::unique_ptr<QuadTree>(oldRoot);
    } else if (x >= rootX && y >= rootY) { // Expand Bottom-Left
      bounds.top += rootHeight;
      bounds.right += rootWidth;
      _root->_nodes[2] = std::unique_ptr<QuadTree>(oldRoot);
    } else if (x <= rootX && y <= rootY) { // Expand Top-Right
      bounds.bottom -= rootHeight;
      bounds.left -= rootWidth;
      _root->_nodes[1] = std::unique_ptr<QuadTree>(oldRoot);
    } else { // Expand Top-Left
      bounds.bottom -= rootHeight;
      bounds.right += rootWidth;
      _root->_nodes[0] = std::unique_ptr<QuadTree>(oldRoot);
    }

    _root->_bounds = bounds;
  }
}

void World::remove(const Entity& entity) {
  auto node = entity.get<WorldNode>();
  node->parent->remove(entity);
}

void World::update() {
  if (_root) {
    _root->update();

    auto empty = 0;

    if (_root->_children.empty()) {
      for (auto& node : _root->_nodes) {
        if (!node || node->isEmpty()) {
          ++empty;
        }
      }
    }

    if (empty == 3) {
      auto oldRoot = _root;

      for (auto& node : oldRoot->_nodes) {
        if (node && !node->isEmpty()) {
          _root = node.release();
          break;
        }
      }

      delete oldRoot;
    }
  }
}

void World::applyToCandidatePairs(const std::function<void(Entity&, Entity&)>& function) {
  if (_root) {
    _root->applyToCandidatePairs(function);
  }
}

}  // namespace linguine::physics
