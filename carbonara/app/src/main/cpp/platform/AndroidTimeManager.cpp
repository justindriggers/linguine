#include "AndroidTimeManager.h"

#include <ctime>

namespace linguine::carbonara {

time_t AndroidTimeManager::currentTime() const {
  timespec now{};
  clock_gettime(CLOCK_MONOTONIC, &now);
  return static_cast<time_t>(now.tv_sec * 1'000'000'000LL + now.tv_nsec);
}

float AndroidTimeManager::durationInSeconds(time_t from, time_t to) const {
    return static_cast<float>(to - from) / 1'000'000'000.0f;
}

}  // namespace linguine::carbonara
