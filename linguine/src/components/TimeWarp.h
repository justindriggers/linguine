#pragma once

namespace linguine {

struct TimeWarp {
  TimeWarp(float factor, float duration)
      : factor(factor), duration(duration) {}

  const float factor = 1.0f;
  const float duration = 1.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
