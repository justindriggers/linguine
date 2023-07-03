#include "WebTimeManager.h"

#include <chrono>

time_t linguine::pesto::WebTimeManager::currentTime() const {
  auto now = std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(now);
}

float linguine::pesto::WebTimeManager::durationInSeconds(time_t from, time_t to) const {
  return difftime(to, from);
}
