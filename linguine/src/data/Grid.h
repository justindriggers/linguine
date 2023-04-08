#pragma once

#include <list>
#include <unordered_map>

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>

namespace linguine {

class Grid {
  public:
    Grid(int width, int height, float scale)
        : _width(width), _height(height), _scale(scale) {}

    [[nodiscard]] glm::vec2 getWorldPosition(glm::vec2 gridPosition) const;

    [[nodiscard]] glm::vec2 getGridPosition(glm::vec2 worldPosition) const;

    void addObstruction(glm::ivec2 location, glm::ivec2 dimensions);

    void removeObstruction(glm::ivec2 location, glm::ivec2 dimensions);

    [[nodiscard]] bool isAdjacent(glm::ivec2 a, glm::ivec2 b) const;

    [[nodiscard]] std::list<glm::ivec2> search(glm::ivec2 start, glm::ivec2 goal);

    [[nodiscard]] int getWidth() const {
      return _width;
    }

    [[nodiscard]] int getHeight() const {
      return _height;
    }

  private:
    struct SearchNode {
      glm::ivec2 position;
      int fScore;

      bool operator>(const SearchNode& other) const {
        return fScore > other.fScore;
      }
    };

    struct Obstruction {
      glm::ivec2 position;
      glm::ivec2 size;
    };

    int _width;
    int _height;
    float _scale;

    std::unordered_map<glm::ivec2, Obstruction> _obstructions;

    [[nodiscard]] std::vector<glm::ivec2> getNeighbors(glm::ivec2 location) const;

    [[nodiscard]] inline bool hasObstruction(glm::ivec2 location) const;

    static inline int heuristic(glm::ivec2 a, glm::ivec2 b);

    [[nodiscard]] static std::list<glm::ivec2> reconstructPath(
        const std::unordered_map<glm::ivec2, glm::ivec2>& cameFrom,
        glm::ivec2 current);
};

}  // namespace linguine
