#pragma once

#include <cstdint>

namespace linguine {

struct LevelTracker {
  LevelTracker(int32_t startXp, int32_t endXp)
      : startXp(startXp), endXp(endXp) {}

  int32_t startXp;
  int32_t endXp;

  float duration = 1.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
