#pragma once

#include <glm/vec3.hpp>

namespace linguine {

class Palette {
  public:
    static constexpr glm::vec3 Black = { 0.0f, 0.0f, 0.0f };
    static constexpr glm::vec3 White = { 1.0f, 1.0f, 1.0f };
    static constexpr glm::vec3 Gray = { 0.78354f, 0.78354f, 0.78354f };
    static constexpr glm::vec3 Orange = { 0.97345f, 0.36625f, 0.00561f };
    static constexpr glm::vec3 Blue = { 0.007f, 0.01521f, 0.04667f };
    static constexpr glm::vec3 Red = { 1.0f, 0.0f, 0.0f };
    static constexpr glm::vec3 Green = { 0.0f, 1.0f, 0.0f };

    static constexpr glm::vec3 Primary = { 0.97345f, 0.36625f, 0.00561f };
    static constexpr glm::vec3 PrimaryAccent = { 0.98225f, 0.47932f, 0.06663f };

    static constexpr glm::vec3 Secondary = { 0.01298f, 0.0319f, 0.10702f };
    static constexpr glm::vec3 SecondaryAccent = { 0.02122f, 0.05448f, 0.19807f };
};

}  // namespace linguine
