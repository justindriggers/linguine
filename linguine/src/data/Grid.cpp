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

void Grid::addObstruction(glm::ivec2 location) {
  _obstructions.insert(location);
}

void Grid::removeObstruction(glm::ivec2 location) {
  _obstructions.erase(location);
}

std::list<glm::ivec2> Grid::search(glm::ivec2 start, glm::ivec2 goal) {
  auto goalIsObstruction = _obstructions.find(goal) != _obstructions.end();

  if (goalIsObstruction) {
    _obstructions.erase(goal);
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

      if (goalIsObstruction) {
        _obstructions.insert(goal);
        result.erase(std::prev(result.end()));
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

  if (goalIsObstruction) {
    _obstructions.insert(goal);
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
