#pragma once

#include "UrlHandler.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "jni/Urls.h"

namespace linguine::carbonara {

class AndroidUrlHandler : public UrlHandler {
  public:
    explicit AndroidUrlHandler(android_app& app) : _urls(app) {}

    void open(const std::string& url) override;

  private:
    Urls _urls;
};

}  // namespace linguine::carbonara
