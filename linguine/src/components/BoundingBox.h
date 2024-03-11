#pragma once

namespace linguine {

struct BoundingBox {
  float top;
  float bottom;
  float left;
  float right;

  [[nodiscard]] bool contains(const BoundingBox& other) const {
    return other.top <= top
           && other.bottom >= bottom
           && other.left >= left
           && other.right <= right;
  }
};

}  // namespace linguine
