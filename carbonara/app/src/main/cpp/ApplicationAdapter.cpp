#include "ApplicationAdapter.h"

#include "OpenGLRenderer.h"
#include "platform/AndroidAudioManager.h"
#include "platform/AndroidInputManager.h"
#include "platform/AndroidLeaderboardManager.h"
#include "platform/AndroidLifecycleManager.h"
#include "platform/AndroidLogger.h"
#include "platform/AndroidOpenGLFileLoader.h"
#include "platform/AndroidSaveManager.h"
#include "platform/AndroidTimeManager.h"

namespace linguine::carbonara {

ApplicationAdapter::ApplicationAdapter(android_app& app) {
  constexpr EGLint attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_NONE
  };

  auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(display, nullptr, nullptr);

  EGLint numConfigs;
  eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

  std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
  eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

  auto config = *std::find_if(
      supportedConfigs.get(),
      supportedConfigs.get() + numConfigs,
      [&display](const EGLConfig &config) {
        EGLint red, green, blue;

        if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
            && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
            && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)) {
          return red == 8 && green == 8 && blue == 8;
        }

        return false;
      });

  EGLint format;
  eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
  EGLSurface surface = eglCreateWindowSurface(display, config, app.window, nullptr);

  EGLint contextAttribs[] = {
      EGL_CONTEXT_MAJOR_VERSION, 3,
      EGL_NONE
  };

  EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);
  eglMakeCurrent(display, surface, surface, context);

  _display = display;
  _surface = surface;
  _context = context;

  auto renderer = std::shared_ptr<render::OpenGLRenderer>(
      render::OpenGLRenderer::create(std::make_unique<AndroidOpenGLFileLoader>())
  );

  auto logger = std::make_shared<AndroidLogger>();
  auto audioManager = std::make_shared<AndroidAudioManager>();
  auto inputManager = std::make_shared<AndroidInputManager>(app, renderer->getViewport());
  auto leaderboardManager = std::make_shared<AndroidLeaderboardManager>();
  auto lifecycleManager = std::make_shared<AndroidLifecycleManager>();

  auto saveManager = std::make_shared<AndroidSaveManager>();
  auto timeManager = std::make_shared<AndroidTimeManager>();

  _engine = std::make_unique<Engine>(logger, audioManager, inputManager,
                                     leaderboardManager, lifecycleManager,
                                     renderer, saveManager, timeManager);
}

ApplicationAdapter::~ApplicationAdapter() {
  if (_display != EGL_NO_DISPLAY) {
    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (_context != EGL_NO_CONTEXT) {
      eglDestroyContext(_display, _context);
      _context = EGL_NO_CONTEXT;
    }

    if (_surface != EGL_NO_SURFACE) {
      eglDestroySurface(_display, _surface);
      _surface = EGL_NO_SURFACE;
    }

    eglTerminate(_display);
    _context = EGL_NO_DISPLAY;
  }
}

void ApplicationAdapter::tick() {
  EGLint width;
  eglQuerySurface(_display, _surface, EGL_WIDTH, &width);

  EGLint height;
  eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);

  auto& renderer = _engine->get<Renderer>();
  auto& viewport = renderer.getViewport();

  if (width != viewport.getWidth() || height != viewport.getHeight()) {
    renderer.resize(width, height);
  }

  _engine->tick();

  eglSwapBuffers(_display, _surface);
}

}  // namespace linguine::carbonara
