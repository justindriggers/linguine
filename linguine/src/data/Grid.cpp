#include "Grid.h"

#include <queue>

#include <glm/gtx/norm.hpp>

namespace linguine {

glm::vec2 Grid::getWorldPosition(glm::vec2 gridPosition) const {
  return {
      (0.5f + gridPosition.x) * _scale,
      (0.5f + gridPosition.y) * _scale
  };
}

glm::vec2 Grid::getGridPosition(glm::vec2 worldPosition) const {
  return {
      worldPosition.x / _scale - 0.5f,
      worldPosition.y / _scale - 0.5f,
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

      if (result.size() > 2) {
        smooth(result);
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

  if (!hasObstruction(left)) {
    results.push_back(left);
  }

  auto right = location + glm::ivec2(1, 0);

  if (!hasObstruction(right)) {
    results.push_back(right);
  }

  auto bottom = location - glm::ivec2(0, 1);

  if (!hasObstruction(bottom)) {
    results.push_back(bottom);
  }

  auto top = location + glm::ivec2(0, 1);

  if (!hasObstruction(top)) {
    results.push_back(top);
  }

  auto bottomLeft = location + glm::ivec2(-1, -1);

  if (!hasObstruction(bottomLeft)
      && !hasObstruction(bottom) && !hasObstruction(left)) {
    results.push_back(bottomLeft);
  }

  auto bottomRight = location + glm::ivec2(1, -1);

  if (!hasObstruction(bottomRight)
      && !hasObstruction(bottom) && !hasObstruction(right)) {
    results.push_back(bottomRight);
  }

  auto topLeft = location + glm::ivec2(-1, 1);

  if (!hasObstruction(topLeft)
      && !hasObstruction(top) && !hasObstruction(left)) {
    results.push_back(topLeft);
  }

  auto topRight = location + glm::ivec2(1, 1);

  if (!hasObstruction(topRight)
      && !hasObstruction(top) && !hasObstruction(right)) {
    results.push_back(topRight);
  }

  return results;
}

bool Grid::isWalkable(glm::ivec2 a, glm::ivec2 b) const {
  const auto start = glm::vec2(a);
  const auto total = glm::distance2(glm::vec2(b), start);
  const auto direction = glm::normalize(glm::vec2(b - a));
  const auto step = 0.2f;

  auto current = start;

  do {
    auto round = glm::ivec2(glm::round(current));

    if (hasObstruction(round)) {
      return false;
    }

    auto floor = glm::ivec2(glm::floor(current));
    auto ceil = glm::ivec2(glm::ceil(current));

    if (round.x == floor.x && hasObstruction({round.x + 1, round.y})) {
      return false;
    }

    if (round.x == ceil.x && hasObstruction({round.x - 1, round.y})) {
      return false;
    }

    if (round.y == floor.y && hasObstruction({round.x, round.y + 1})) {
      return false;
    }

    if (round.y == ceil.y && hasObstruction({round.x, round.y - 1})) {
      return false;
    }

    if (round.x == floor.x && round.y == floor.y
        && hasObstruction({round.x + 1, round.y + 1})) {
      return false;
    }

    if (round.x == ceil.x && round.y == ceil.y
        && hasObstruction({round.x - 1, round.y - 1})) {
      return false;
    }

    if (round.x == floor.x && round.y == ceil.y
        && hasObstruction({round.x + 1, round.y - 1})) {
      return false;
    }

    if (round.x == ceil.x && round.y == floor.y
        && hasObstruction({round.x - 1, round.y + 1})) {
      return false;
    }

    current += direction * step;
  } while (glm::distance2(current, start) <= total);

  return true;
}

void Grid::smooth(std::list<glm::ivec2>& path) const {
  auto previous = path.begin();
  auto current = std::next(previous);
  std::list<glm::ivec2>::iterator next;

  while ((next = std::next(current)) != path.end()) {
    if (isWalkable(*previous, *next)) {
      auto toRemove = current;
      current = std::next(current);
      path.erase(toRemove);
    } else {
      previous = current;
      current = std::next(current);
    }
  }
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
