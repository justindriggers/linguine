#include "Grid.h"

#include <queue>

#include <glm/gtx/norm.hpp>

namespace linguine {

glm::vec2 Grid::getWorldPosition(glm::vec2 gridPosition) const {
  return {
      (-static_cast<float>(_width) / 2.0f + 0.5f + gridPosition.x) * _scale,
      (-static_cast<float>(_height) / 2.0f + 0.5f + gridPosition.y) * _scale
  };
}

glm::vec2 Grid::getGridPosition(glm::vec2 worldPosition) const {
  return {
      worldPosition.x / _scale + static_cast<float>(_width) / 2.0f - 0.5f,
      worldPosition.y / _scale + static_cast<float>(_height) / 2.0f - 0.5f,
  };
}

void Grid::addObstruction(glm::ivec2 location, glm::ivec2 dimensions) {
  auto obstruction = Obstruction{location, dimensions};
  for (auto x = location.x; x < location.x + dimensions.x; ++x) {
    for (auto y = location.y; y < location.y + dimensions.y; ++y) {
      _obstructions.insert({{x, y}, obstruction});
    }
  }
}

void Grid::removeObstruction(glm::ivec2 location, glm::ivec2 dimensions) {
  for (auto x = location.x; x < location.x + dimensions.x; ++x) {
    for (auto y = location.y; y < location.y + dimensions.y; ++y) {
      _obstructions.erase({x, y});
    }
  }
}

bool Grid::isAdjacent(glm::ivec2 a, glm::ivec2 b) const {
  return (a.x == b.x || a.x == b.x + 1 || a.x == b.x - 1)
         && (a.y == b.y || a.y == b.y + 1 || a.y == b.y - 1);
}

std::list<glm::ivec2> Grid::search(glm::ivec2 start, glm::ivec2 goal) {
  auto goalObstruction = std::optional<Obstruction>();
  auto goalObstructionIt = _obstructions.find(goal);

  if (goalObstructionIt != _obstructions.end()) {
    goalObstruction = goalObstructionIt->second;
    for (auto x = goalObstruction->position.x; x < goalObstruction->position.x + goalObstruction->size.x; ++x) {
      for (auto y = goalObstruction->position.y; y < goalObstruction->position.y + goalObstruction->size.y; ++y) {
        _obstructions.erase({x, y});
      }
    }
  }

  auto openSet = std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<>>();

  auto startGScore = 0;
  auto startFScore = startGScore + heuristic(start, goal);

  openSet.push(SearchNode{ start, startFScore });

  auto cameFrom = std::unordered_map<glm::ivec2, glm::ivec2>();

  auto gScores = std::unordered_map<glm::ivec2, int> {
      { start, startGScore }
  };

  auto fScores = std::unordered_map<glm::ivec2, int> {
      { start, startFScore }
  };

  while (!openSet.empty()) {
    auto current = openSet.top();

    if (current.position == goal) {
      auto result = reconstructPath(cameFrom, current.position);

      if (goalObstruction) {
        for (auto x = goalObstruction->position.x; x < goalObstruction->position.x + goalObstruction->size.x; ++x) {
          for (auto y = goalObstruction->position.y; y < goalObstruction->position.y + goalObstruction->size.y; ++y) {
            _obstructions.insert({{x, y}, *goalObstruction});
          }
        }

        while (result.size() > 1) {
          auto it = std::prev(result.end());

          if (_obstructions.find(*it) == _obstructions.end()) {
            break;
          }

          result.erase(it);
        }
      }

      return result;
    }

    openSet.pop();

    for (const auto& neighbor : getNeighbors(current.position)) {
      auto tentativeGScore = gScores[current.position] + 1;

      auto neighborGScore = gScores.find(neighbor);
      if (neighborGScore == gScores.end() || tentativeGScore < neighborGScore->second) {
        cameFrom.insert({ neighbor, current.position });

        auto gScore = tentativeGScore;
        auto fScore = gScore + heuristic(neighbor, goal);

        gScores.insert({ neighbor, gScore });
        fScores.insert({ neighbor, fScore });

        openSet.push(SearchNode{ neighbor, fScore });
      }
    }
  }

  if (goalObstruction) {
    for (auto x = goalObstruction->position.x; x < goalObstruction->position.x + goalObstruction->size.x; ++x) {
      for (auto y = goalObstruction->position.y; y < goalObstruction->position.y + goalObstruction->size.y; ++y) {
        _obstructions.insert({{x, y}, *goalObstruction});
      }
    }
  }

  return {};
}

std::vector<glm::ivec2> Grid::getNeighbors(glm::ivec2 location) const {
  auto results = std::vector<glm::ivec2>();

  auto left = location - glm::ivec2(1, 0);

  if (location.x > 0 && !hasObstruction(left)) {
    results.push_back(left);
  }

  auto right = location + glm::ivec2(1, 0);

  if (location.x < _width - 1 && !hasObstruction(right)) {
    results.push_back(right);
  }

  auto bottom = location - glm::ivec2(0, 1);

  if (location.y > 0 && !hasObstruction(bottom)) {
    results.push_back(bottom);
  }

  auto top = location + glm::ivec2(0, 1);

  if (location.y < _height - 1 && !hasObstruction(top)) {
    results.push_back(top);
  }

  auto bottomLeft = location + glm::ivec2(-1, -1);

  if (location.x > 0 && location.y > 0 && !hasObstruction(bottomLeft)
      && !hasObstruction(bottom) && !hasObstruction(left)) {
    results.push_back(bottomLeft);
  }

  auto bottomRight = location + glm::ivec2(1, -1);

  if (location.x < _width - 1 && location.y > 0 && !hasObstruction(bottomRight)
      && !hasObstruction(bottom) && !hasObstruction(right)) {
    results.push_back(bottomRight);
  }

  auto topLeft = location + glm::ivec2(-1, 1);

  if (location.x > 0 && location.y < _height - 1 && !hasObstruction(topLeft)
      && !hasObstruction(top) && !hasObstruction(left)) {
    results.push_back(topLeft);
  }

  auto topRight = location + glm::ivec2(1, 1);

  if (location.x < _width - 1 && location.y < _height - 1 && !hasObstruction(topRight)
      && !hasObstruction(top) && !hasObstruction(right)) {
    results.push_back(topRight);
  }

  return results;
}

bool Grid::hasObstruction(glm::ivec2 location) const {
  return _obstructions.find(location) != _obstructions.end();
}

int Grid::heuristic(glm::ivec2 a, glm::ivec2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::list<glm::ivec2> Grid::reconstructPath(
    const std::unordered_map<glm::ivec2, glm::ivec2>& cameFrom,
    glm::ivec2 current) {
  auto path = std::list<glm::ivec2> { current };

  std::unordered_map<glm::ivec2, glm::ivec2>::const_iterator it;
  while ((it = cameFrom.find(current)) != cameFrom.end()) {
    current = it->second;
    path.insert(path.begin(), current);
  }

  return path;
}

}  // namespace linguine
