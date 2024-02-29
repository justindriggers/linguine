#include "ApplicationAdapter.h"

#include "platform/AndroidInputManager.h"

using namespace linguine::carbonara;

void handle_cmd(android_app* app, int32_t cmd) {
  switch (cmd) {
  case APP_CMD_INIT_WINDOW: {
    if (!app->userData) {
      app->userData = new ApplicationAdapter(*app);
    } else {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      applicationAdapter->onInitWindow(*app);
    }
    break;
  }
  case APP_CMD_GAINED_FOCUS:
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      applicationAdapter->onResume();
    }
    break;
  case APP_CMD_LOST_FOCUS:
  case APP_CMD_PAUSE:
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      applicationAdapter->onPause();
    }
    break;
  case APP_CMD_DESTROY:
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      app->userData = nullptr;
      delete applicationAdapter;
    }
    break;
  case APP_CMD_TERM_WINDOW:
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      applicationAdapter->onTerminateWindow();
    }
    break;
  default:
    break;
  }
}

void android_main(struct android_app* app) {
  app->onAppCmd = handle_cmd;

  do {
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);

      if (applicationAdapter->isPaused()) {
        AndroidInputManager::pollNativeEvents(*app);
      } else {
        applicationAdapter->tick();
      }
    } else {
      AndroidInputManager::pollNativeEvents(*app);
    }
  } while (!app->destroyRequested);
}
