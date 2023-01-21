#include "MacTimeManager.h"

time_t linguine::alfredo::MacTimeManager::currentTime() const {
  return static_cast<time_t>(clock_gettime_nsec_np(CLOCK_UPTIME_RAW));
}

float linguine::alfredo::MacTimeManager::durationInSeconds(time_t from, time_t to) const {
  return static_cast<float>(to - from) / 1'000'000'000.0f;
}
