#pragma once

#include <Engine.h>

#include <EGL/egl.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace linguine::carbonara {

class ApplicationAdapter {
  public:
    explicit ApplicationAdapter(android_app& app);

    ~ApplicationAdapter();

    void tick();

    void onInitWindow(android_app& app);

    void onPause();

    void onResume();

    void onTerminateWindow();

    [[nodiscard]] bool isPaused() const {
      return _isPaused;
    }

  private:
    std::unique_ptr<Engine> _engine;

    bool _isPaused = false;

    EGLConfig _config = nullptr;
    EGLDisplay _display = EGL_NO_DISPLAY;
    EGLSurface _surface = EGL_NO_SURFACE;
    EGLContext _context = EGL_NO_CONTEXT;

    void createSurface(android_app& app);

    void createContext();
};

}  // namespace linguine::carbonara
