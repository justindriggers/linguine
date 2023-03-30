#pragma once

#include <functional>

namespace linguine {

struct Event {
  float secondsRemaining = 0.0f;
  std::function<void()> function = {};
  bool useFixedTimeStep = false;
};

}  // namespace linguine
