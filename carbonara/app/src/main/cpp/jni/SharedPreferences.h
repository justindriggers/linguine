#pragma once

#include <string>

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace linguine::carbonara {

class SharedPreferences {
  public:
    class Editor {
      public:
        Editor(JNIEnv& env, jobject editor);

        ~Editor();

        Editor& putBoolean(std::string key, bool value);

        Editor& putInt(std::string key, int32_t value);

        void apply();

      private:
        JNIEnv& _env;
        jobject _editor;

        jmethodID _putBooleanMethod{};
        jmethodID _putIntMethod{};
        jmethodID _applyMethod{};
    };

    enum Mode {
        Private = 0
    };

    explicit SharedPreferences(android_app& app, std::string name, Mode mode);

    ~SharedPreferences();

    Editor edit();

    bool getBoolean(std::string key, bool defaultValue);

    int32_t getInt(std::string key, int32_t defaultValue);

  private:
    android_app& _app;
    JNIEnv* _env{};

    jobject _sharedPreferences{};
    jmethodID _editMethod{};
    jmethodID _getIntMethod{};
    jmethodID _getBooleanMethod{};
};

}  // namespace linguine::carbonara
