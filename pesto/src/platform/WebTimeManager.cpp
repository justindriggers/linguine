#include "WebTimeManager.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

namespace linguine::pesto {

WebTimeManager::WebTimeManager() : TimeManager(currentTime()) {
  emscripten_set_visibilitychange_callback(this, false, [](int eventType, const EmscriptenVisibilityChangeEvent* visibilityChangeEvent, void* userData) -> EM_BOOL {
    auto& timeManager = *static_cast<WebTimeManager*>(userData);

    if (visibilityChangeEvent->visibilityState == EMSCRIPTEN_VISIBILITY_VISIBLE) {
      timeManager.setTimeScale(1.0f);
      timeManager.reset();
    } else {
      timeManager.setTimeScale(0.0f);
    }

    return true;
  });
}

time_t WebTimeManager::currentTime() const {
  return static_cast<time_t>(emscripten_get_now() * 1'000'000.0f);
}

float WebTimeManager::durationInSeconds(time_t from, time_t to) const {
  return static_cast<float>(to - from) / 1'000'000'000.0f;
}

}  // namespace linguine::pesto
