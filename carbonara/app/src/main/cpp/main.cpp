#include "ApplicationAdapter.h"

#include "platform/AndroidInputManager.h"

using namespace linguine::carbonara;

void handle_cmd(android_app* app, int32_t cmd) {
  switch (cmd) {
  case APP_CMD_INIT_WINDOW:
    app->userData = new ApplicationAdapter(*app);
    break;
  case APP_CMD_TERM_WINDOW:
    if (app->userData) {
      auto* applicationAdapter = reinterpret_cast<ApplicationAdapter*>(app->userData);
      app->userData = nullptr;
      delete applicationAdapter;
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
      reinterpret_cast<ApplicationAdapter*>(app->userData)->tick();
    } else {
      AndroidInputManager::pollNativeEvents(*app);
    }
  } while (!app->destroyRequested);
}
