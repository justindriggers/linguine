#include "QuadTree.h"

#include <vector>

#include "World.h"
#include "components/WorldNode.h"
#include "components/Static.h"

namespace linguine::physics {

bool QuadTree::insert(Entity& entity) {
  auto boundingBox = entity.get<BoundingBox>();

  if (_bounds.contains(*boundingBox)) {
    for (auto i = 0; i < _nodes.size(); ++i) {
      if (!_nodes[i]) {
        auto top = _bounds.top;
        auto bottom = _bounds.bottom;
        auto left = _bounds.left;
        auto right = _bounds.right;

        auto halfHeight = (top - bottom) / 2.0f;
        auto halfWidth = (right - left) / 2.0f;

        switch (i) {
        case 0: // Top-Left
          bottom += halfHeight;
          right -= halfWidth;
          break;
        case 1: // Top-Right
          bottom += halfHeight;
          left += halfWidth;
          break;
        case 2: // Bottom-Left
          top -= halfHeight;
          right -= halfWidth;
          break;
        case 3: // Bottom-Right
          top -= halfHeight;
          left += halfWidth;
          break;
        default:
          break;
        }

        auto subBox = BoundingBox { top, bottom, left, right };
        _nodes[i] = std::make_unique<QuadTree>(_world, _entityManager, subBox);
      }

      if (_nodes[i]->insert(entity)) {
        return true;
      }
    }

    _children.insert(entity.getId());

    auto worldNode = entity.get<WorldNode>();
    worldNode->parent = this;

    return true;
  }

  return false;
}

void QuadTree::remove(const Entity& entity) {
  if (_children.find(entity.getId()) != _children.end()) {
    _children.erase(entity.getId());
  }
}

bool QuadTree::isEmpty() {
  return _children.empty()
         && !_nodes[0]
         && !_nodes[1]
         && !_nodes[2]
         && !_nodes[3];
}

void QuadTree::update() {
  auto pushed = std::vector<uint64_t>();
  auto pulled = std::vector<uint64_t>();

  for (const auto& child : _children) {
    auto entity = _entityManager.getById(child);

    if (entity->has<Static>()) continue;

    auto boundingBox = entity->get<BoundingBox>();

    if (_bounds.contains(*boundingBox)) {
      for (const auto& node : _nodes) {
        if (node->insert(*entity)) {
          pushed.push_back(child);
          break;
        }
      }
    } else {
      pulled.push_back(child);
    }
  }

  for (const auto& child : pushed) {
    _children.erase(child);
  }

  for (const auto& child : pulled) {
    _children.erase(child);

    auto entity = _entityManager.getById(child);
    _world.add(*entity);
  }

  if (_children.empty()) {
    if (_nodes[0] && _nodes[0]->isEmpty()) {
      _nodes[0] = {};
    }
    if (_nodes[1] && _nodes[1]->isEmpty()) {
      _nodes[1] = {};
    }
    if (_nodes[2] && _nodes[2]->isEmpty()) {
      _nodes[2] = {};
    }
    if (_nodes[3] && _nodes[3]->isEmpty()) {
      _nodes[3] = {};
    }
  }

  for (auto& node : _nodes) {
    if (node) {
      node->update();
    }
  }
}

void QuadTree::applyToCandidatePairs(const std::function<void(Entity& a, Entity& b)>& function) {
  for (const auto& a : _children) {
    applyTo(function, *_entityManager.getById(a));
  }

  for (const auto& node : _nodes) {
    if (!node) continue;
    node->applyToCandidatePairs(function);
  }
}

void QuadTree::applyTo(const std::function<void(Entity&, Entity&)>& function,
                       Entity& a) {
  for (const auto& b : _children) {
    if (a.getId() == b) continue;
    function(a, *_entityManager.getById(b));
  }

  for (const auto& node : _nodes) {
    if (!node) continue;
    node->applyTo(function, a);
  }
}

}  // namespace linguine::physics
