#include "AndroidInputManager.h"

namespace linguine::carbonara {

void AndroidInputManager::pollEvents() {
  pollNativeEvents(_app);
}

void AndroidInputManager::pollNativeEvents(android_app& app) {
  int events;
  android_poll_source* source;

  if (ALooper_pollAll(0, nullptr, &events, (void **) &source) >= 0) {
    if (source) {
      source->process(&app, source);
    }
  }
}

}  // namespace linguine::carbonara
