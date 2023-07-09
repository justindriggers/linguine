#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <Engine.h>
#include <OpenGLRenderer.h>

#include "platform/WebAudioManager.h"
#include "platform/WebInputManager.h"
#include "platform/WebLifecycleManager.h"
#include "platform/WebLogger.h"
#include "platform/WebTimeManager.h"

using namespace linguine;
using namespace linguine::pesto;
using namespace linguine::render;

std::unique_ptr<Engine> engine;

inline void tick() {
  engine->tick();
}

int main() {
  auto logger = std::make_shared<WebLogger>();
  auto audioManager = std::make_shared<WebAudioManager>();
  auto inputManager = std::make_shared<WebInputManager>();
  auto lifecycleManager = std::make_shared<WebLifecycleManager>();
  auto timeManager = std::make_shared<WebTimeManager>();

  EmscriptenWebGLContextAttributes attributes;
  emscripten_webgl_init_context_attributes(&attributes);

  attributes.majorVersion = 2;
  attributes.minorVersion = 0;

  emscripten_set_canvas_element_size("canvas", 1440, 768);

  auto contextHandle = emscripten_webgl_create_context("canvas", &attributes);

  if (contextHandle < 0) {
    logger->log("Error " + std::to_string(contextHandle) + " when creating WebGL context");
    return -1;
  }

  emscripten_webgl_make_context_current(contextHandle);

  auto renderer = std::shared_ptr<OpenGLRenderer>(OpenGLRenderer::create());
  renderer->resize(1440, 768);

  engine = std::make_unique<Engine>(logger, audioManager, inputManager,
                                    lifecycleManager, renderer, timeManager);

  emscripten_set_main_loop(tick, 0, false);
  return 0;
}
