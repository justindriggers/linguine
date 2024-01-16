#include "WebTimeManager.h"

#include <emscripten/emscripten.h>

namespace linguine::pesto {

time_t WebTimeManager::currentTime() const {
  return static_cast<time_t>(emscripten_get_now() * 1'000'000.0f);
}

float WebTimeManager::durationInSeconds(time_t from, time_t to) const {
  return static_cast<float>(to - from) / 1'000'000'000.0f;
}

}  // namespace linguine::pesto
