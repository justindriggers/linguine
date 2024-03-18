#include "Urls.h"

namespace linguine::carbonara {

Urls::Urls(android_app& app) : _app(app) {
  auto result = _app.activity->vm->GetEnv(reinterpret_cast<void**>(&_env), JNI_VERSION_1_6);

  if (result == JNI_EDETACHED) {
    result = _app.activity->vm->AttachCurrentThread(&_env, nullptr);
  }

  if (result != JNI_OK) {
    throw std::runtime_error("Error retrieving JNI environment");
  }

  auto gameActivityClass = _env->GetObjectClass(_app.activity->javaGameActivity);
  _openUrlMethod = _env->GetMethodID(gameActivityClass, "openUrl",
                                     "(Ljava/lang/String;)V");

  if (!_openUrlMethod) {
    throw std::runtime_error("Invalid method: openUrl(Ljava/lang/String;)V");
  }
}

void Urls::open(const std::string& url) {
  _env->CallVoidMethod(_app.activity->javaGameActivity, _openUrlMethod,
                       _env->NewStringUTF(url.data()));
}

}  // namespace linguine::carbonara
