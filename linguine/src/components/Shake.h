#pragma once

namespace linguine {

struct Shake {
  Shake(float magnitude, float duration)
      : magnitude(magnitude), duration(duration) {}

  const float magnitude = 0.0f;
  const float duration = 0.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
