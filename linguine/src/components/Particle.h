#pragma once

namespace linguine {

struct Particle {
  float duration = 1.0f;
  float elapsed = 0.0f;
  float scalePerSecond = -0.25f;
};

}  // namespace linguine
