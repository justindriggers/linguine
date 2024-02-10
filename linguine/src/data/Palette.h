#pragma once

#include <glm/vec4.hpp>

namespace linguine {

class Palette {
  public:
    static constexpr glm::vec4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
    static constexpr glm::vec4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr glm::vec4 Gray = { 0.78354f, 0.78354f, 0.78354f, 1.0f };
    static constexpr glm::vec4 Orange = { 0.97345f, 0.36625f, 0.00561f, 1.0f };
    static constexpr glm::vec4 Navy = { 0.007f, 0.01521f, 0.04667f, 1.0f };

    static constexpr glm::vec4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
    static constexpr glm::vec4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr glm::vec4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    static constexpr glm::vec4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr glm::vec4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
    static constexpr glm::vec4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };

    static constexpr glm::vec4 Primary = { 0.97345f, 0.36625f, 0.00561f, 1.0f };
    static constexpr glm::vec4 PrimaryAccent = { 0.98225f, 0.47932f, 0.06663f, 1.0f };

    static constexpr glm::vec4 Secondary = { 0.01298f, 0.0319f, 0.10702f, 1.0f };
    static constexpr glm::vec4 SecondaryAccent = { 0.02122f, 0.05448f, 0.19807f, 1.0f };
};

}  // namespace linguine
