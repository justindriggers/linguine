#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <Engine.h>
#include <OpenGLRenderer.h>

#include "platform/WebAudioManager.h"
#include "platform/WebInputManager.h"
#include "platform/WebLifecycleManager.h"
#include "platform/WebLogger.h"
#include "platform/WebTimeManager.h"

namespace linguine::pesto {

auto logger = std::make_shared<WebLogger>();
auto audioManager = std::make_shared<WebAudioManager>();
auto inputManager = std::make_shared<WebInputManager>();
auto lifecycleManager = std::make_shared<WebLifecycleManager>();
auto renderer = std::make_shared<linguine::render::OpenGLRenderer>();
auto timeManager = std::make_shared<WebTimeManager>();

Engine engine = Engine(logger, audioManager, inputManager, lifecycleManager,
                       renderer, timeManager);

inline void tick() {
  engine.tick();
}

}  // namespace linguine::pesto

int main() {
  auto& logger = linguine::pesto::engine.get<linguine::Logger>();

  EmscriptenWebGLContextAttributes attributes;
  emscripten_webgl_init_context_attributes(&attributes);

  attributes.majorVersion = 2;
  attributes.minorVersion = 0;

  auto contextHandle = emscripten_webgl_create_context("canvas", &attributes);

  if (contextHandle < 0) {
    logger.log("Error " + std::to_string(contextHandle) + " when creating WebGL context");
    return -1;
  }

  emscripten_webgl_make_context_current(contextHandle);

  emscripten_set_main_loop(linguine::pesto::tick, 0, false);
  return 0;
}
