#include "ApplicationAdapter.h"

#include <AAudioAudioManager.h>
#include <OpenGLRenderBackend.h>

#include "platform/AndroidAAudioFileLoader.h"
#include "platform/AndroidInputManager.h"
#include "platform/AndroidLeaderboardManager.h"
#include "platform/AndroidLifecycleManager.h"
#include "platform/AndroidLogger.h"
#include "platform/AndroidOpenGLFileLoader.h"
#include "platform/AndroidSaveManager.h"
#include "platform/AndroidTimeManager.h"

namespace linguine::carbonara {

ApplicationAdapter::ApplicationAdapter(android_app& app) {
  _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(_display, nullptr, nullptr);

  constexpr EGLint attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_NONE
  };

  EGLint numConfigs;
  eglChooseConfig(_display, attribs, nullptr, 0, &numConfigs);

  auto supportedConfigs = std::vector<EGLConfig>(numConfigs);
  eglChooseConfig(_display, attribs, supportedConfigs.data(), numConfigs, &numConfigs);

  _config = *std::find_if(supportedConfigs.begin(), supportedConfigs.end(),
                          [this](const EGLConfig& config) {
                            EGLint red, green, blue;

                            if (eglGetConfigAttrib(_display, config, EGL_RED_SIZE, &red)
                                && eglGetConfigAttrib(_display, config, EGL_GREEN_SIZE, &green)
                                && eglGetConfigAttrib(_display, config, EGL_BLUE_SIZE, &blue)) {
                              return red == 8 && green == 8 && blue == 8;
                            }

                            return false;
                          });

  createSurface(app);
  createContext();

  eglMakeCurrent(_display, _surface, _surface, _context);

  auto renderBackend = render::OpenGLRenderBackend::create(std::make_unique<AndroidOpenGLFileLoader>(*app.activity->assetManager));
  auto renderer = std::make_shared<Renderer>(std::move(renderBackend));
  
  auto audioManager = std::make_shared<audio::AAudioAudioManager>(
      std::make_unique<AndroidAAudioFileLoader>(*app.activity->assetManager)
  );

  auto logger = std::make_shared<AndroidLogger>();
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
    _display = EGL_NO_DISPLAY;
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

void ApplicationAdapter::onInitWindow(android_app& app) {
  auto& renderer = _engine->get<Renderer>();

  createSurface(app);

  auto result = eglMakeCurrent(_display, _surface, _surface, _context);

  if (!result) {
    auto error = eglGetError();

    if (error == EGL_CONTEXT_LOST) {
      createContext();

      eglMakeCurrent(_display, _surface, _surface, _context);

      auto renderBackend = render::OpenGLRenderBackend::create(std::make_unique<AndroidOpenGLFileLoader>(*app.activity->assetManager));
      renderer.setBackend(std::move(renderBackend));
    } else {
      auto message = "Error initializing window: " + std::to_string(error);
      _engine->get<Logger>().log(message);
      throw std::runtime_error(message);
    }
  }

  EGLint width;
  eglQuerySurface(_display, _surface, EGL_WIDTH, &width);

  EGLint height;
  eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);

  renderer.resize(width, height);
}

void ApplicationAdapter::onPause() {
  _isPaused = true;
  _engine->pause();
}

void ApplicationAdapter::onResume() {
  _isPaused = false;
  _engine->resume();
}

void ApplicationAdapter::onTerminateWindow() {
  if (_surface != EGL_NO_SURFACE) {
    eglDestroySurface(_display, _surface);
    _surface = EGL_NO_SURFACE;
  }
}

void ApplicationAdapter::createSurface(android_app& app) {
  _surface = eglCreateWindowSurface(_display, _config, app.window, nullptr);
}

void ApplicationAdapter::createContext() {
  EGLint contextAttribs[] = {
      EGL_CONTEXT_MAJOR_VERSION, 3,
      EGL_NONE
  };

  _context = eglCreateContext(_display, _config, nullptr, contextAttribs);
}

}  // namespace linguine::carbonara
