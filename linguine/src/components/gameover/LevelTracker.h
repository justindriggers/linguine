#pragma once

#include <cstdint>

namespace linguine {

struct LevelTracker {
  LevelTracker(int32_t startXp, int32_t endXp)
      : startXp(startXp), endXp(endXp), lastAudibleXp(startXp) {}

  int32_t startXp;
  int32_t endXp;

  float duration = 1.0f;
  float elapsed = 0.0f;
  float lastAudible = 0.0f;
  int32_t lastAudibleXp = 0;
};

}  // namespace linguine
