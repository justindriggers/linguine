#pragma once

#include <string>

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace linguine::carbonara {

class Urls {
  public:
    explicit Urls(android_app& app);

    void open(const std::string& url);

  private:
    android_app& _app;
    JNIEnv* _env{};

    jmethodID _openUrlMethod{};
};

}  // namespace linguine::carbonara
