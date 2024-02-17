#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <Engine.h>
#include <OpenALAudioManager.h>
#include <OpenGLRenderer.h>

#include "platform/WebInputManager.h"
#include "platform/WebLeaderboardManager.h"
#include "platform/WebLifecycleManager.h"
#include "platform/WebLogger.h"
#include "platform/WebOpenALFileLoader.h"
#include "platform/WebOpenGLFileLoader.h"
#include "platform/WebSaveManager.h"
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
  auto leaderboardManager = std::make_shared<WebLeaderboardManager>();
  auto lifecycleManager = std::make_shared<WebLifecycleManager>();
  auto saveManager = std::make_shared<WebSaveManager>();
  auto timeManager = std::make_shared<WebTimeManager>();
  auto audioManager = std::make_shared<audio::OpenALAudioManager>(
      std::make_unique<WebOpenALFileLoader>(), *timeManager);
  audioManager->pause();

  EmscriptenWebGLContextAttributes attributes;
  emscripten_webgl_init_context_attributes(&attributes);

  attributes.majorVersion = 2;
  attributes.minorVersion = 0;
  attributes.depth = false;
  attributes.antialias = false;

  int width, height;
  emscripten_get_canvas_element_size("canvas", &width, &height);

  if (width == 300 && height == 150) {
    width = 540;
    height = 900;

    emscripten_set_canvas_element_size("canvas", width, height);
  }

  auto contextHandle = emscripten_webgl_create_context("canvas", &attributes);

  if (contextHandle < 0) {
    logger->log("Error " + std::to_string(contextHandle) + " when creating WebGL context");
    return -1;
  }

  emscripten_webgl_make_context_current(contextHandle);

  auto renderer = std::shared_ptr<OpenGLRenderer>(OpenGLRenderer::create(
      std::make_unique<WebOpenGLFileLoader>()));
  renderer->resize(width, height);

  auto inputManager = std::make_shared<WebInputManager>(renderer->getViewport(), *audioManager);

  engine = std::make_unique<Engine>(logger, audioManager, inputManager,
                                    leaderboardManager, lifecycleManager,
                                    renderer, saveManager, timeManager);

  emscripten_set_visibilitychange_callback(engine.get(), false, [](int eventType, const EmscriptenVisibilityChangeEvent* visibilityChangeEvent, void* userData) -> EM_BOOL {
    auto& engine = *static_cast<Engine*>(userData);

    if (visibilityChangeEvent->visibilityState == EMSCRIPTEN_VISIBILITY_VISIBLE) {
      engine.resume();
    } else {
      engine.pause();
    }

    return true;
  });

  emscripten_set_main_loop(tick, 0, false);
  return 0;
}
