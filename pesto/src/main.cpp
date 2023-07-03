#include <emscripten/emscripten.h>

#include <Engine.h>

#include "platform/WebAudioManager.h"
#include "platform/WebInputManager.h"
#include "platform/WebLifecycleManager.h"
#include "platform/WebLogger.h"
#include "platform/WebRenderer.h"
#include "platform/WebTimeManager.h"

namespace linguine::pesto {

auto logger = std::make_shared<WebLogger>();
auto audioManager = std::make_shared<WebAudioManager>();
auto inputManager = std::make_shared<WebInputManager>();
auto lifecycleManager = std::make_shared<WebLifecycleManager>();
auto renderer = std::make_shared<WebRenderer>();
auto timeManager = std::make_shared<WebTimeManager>();

Engine engine = Engine(logger, audioManager, inputManager, lifecycleManager,
                       renderer, timeManager);

inline void tick() {
  engine.tick();
}

}  // namespace linguine::pesto

int main() {
  emscripten_set_main_loop(linguine::pesto::tick, 0, false);
  return 0;
}
