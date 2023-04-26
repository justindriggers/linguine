#pragma once

#include <vector>

#include <glm/vec2.hpp>

namespace linguine {

class RoomLayout {
  public:
    void add(glm::ivec2 location, glm::ivec2 dimensions) {
      ensureCapacity(location, dimensions);

      for (auto x = 0; x < dimensions.x; ++x) {
        for (auto y = 0; y < dimensions.y; ++y) {
          _cells[location.x + x][location.y + y] = true;
        }
      }
    }

    void remove(glm::ivec2 location, glm::ivec2 dimensions) {
      ensureCapacity(location, dimensions);

      for (auto x = 0; x < dimensions.x; ++x) {
        for (auto y = 0; y < dimensions.y; ++y) {
          _cells[location.x + x][location.y + y] = false;
        }
      }
    }

    [[nodiscard]] bool isOccupied(glm::ivec2 location) const {
      if (_cells.size() <= location.x || _cells[location.x].size() < location.y) {
        return false;
      }

      return _cells[location.x][location.y];
    }

  private:
    std::vector<std::vector<bool>> _cells;

    void ensureCapacity(glm::ivec2 location, glm::ivec2 dimensions) {
      if (_cells.size() < location.x + dimensions.x) {
        _cells.resize(location.x + dimensions.x);
      }

      for (auto x = 0; x < dimensions.x; ++x) {
        if (_cells[location.x + x].size() < location.y + dimensions.y) {
          _cells[location.x + x].resize(location.y + dimensions.y);
        }
      }
    }
};

}  // namespace linguine
