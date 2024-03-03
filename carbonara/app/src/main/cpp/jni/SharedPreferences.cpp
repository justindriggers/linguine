#include "SharedPreferences.h"

#include <stdexcept>

#include <jni.h>

namespace linguine::carbonara {

SharedPreferences::SharedPreferences(android_app& app, std::string name, Mode mode)
    : _app(app) {
  auto result = _app.activity->vm->GetEnv(reinterpret_cast<void**>(&_env), JNI_VERSION_1_6);

  if (result == JNI_EDETACHED) {
    result = _app.activity->vm->AttachCurrentThread(&_env, nullptr);
  }

  if (result != JNI_OK) {
    throw std::runtime_error("Error retrieving JNI environment");
  }

  auto gameActivityClass = _env->GetObjectClass(_app.activity->javaGameActivity);
  auto getSharedPreferencesMethod = _env->GetMethodID(gameActivityClass, "getSharedPreferences",
                                                      "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");

  if (!getSharedPreferencesMethod) {
    throw std::runtime_error("Invalid method: getSharedPreferences(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
  }

  _sharedPreferences = _env->CallObjectMethod(_app.activity->javaGameActivity,//0x736d2c4721
                                              getSharedPreferencesMethod,
                                              _env->NewStringUTF(name.data()), mode);

  auto sharedPreferencesClass = _env->GetObjectClass(_sharedPreferences);
  _editMethod = _env->GetMethodID(sharedPreferencesClass, "edit",
                                  "()Landroid/content/SharedPreferences$Editor;");

  if (!_editMethod) {
    throw std::runtime_error("Invalid method: edit()Landroid/content/SharedPreferences$Editor;");
  }

  _getBooleanMethod = _env->GetMethodID(sharedPreferencesClass, "getBoolean",
                                        "(Ljava/lang/String;Z)Z");

  if (!_getBooleanMethod) {
    throw std::runtime_error("Invalid method: getBoolean(Ljava/lang/String;Z)Z");
  }

  _getIntMethod = _env->GetMethodID(sharedPreferencesClass, "getInt",
                                    "(Ljava/lang/String;I)I");

  if (!_getIntMethod) {
    throw std::runtime_error("Invalid method: getInt(Ljava/lang/String;I)I");
  }
}

SharedPreferences::Editor SharedPreferences::edit() {
  auto editor = _env->CallObjectMethod(_sharedPreferences, _editMethod);
  return { *_env, editor };
}

bool SharedPreferences::getBoolean(std::string key, bool defaultValue) {
  return _env->CallBooleanMethod(_sharedPreferences, _getBooleanMethod,
                                 _env->NewStringUTF(key.data()), defaultValue);
}

int32_t SharedPreferences::getInt(std::string key, int32_t defaultValue) {
  return _env->CallIntMethod(_sharedPreferences, _getIntMethod,
                             _env->NewStringUTF(key.data()), defaultValue);
}

SharedPreferences::Editor::Editor(JNIEnv& env, jobject editor)
    : _editor(editor), _env(env) {
  auto editorClass = _env.GetObjectClass(_editor);

  _putBooleanMethod = _env.GetMethodID(editorClass, "putBoolean",
                                       "(Ljava/lang/String;Z)Landroid/content/SharedPreferences$Editor;");

  if (!_putBooleanMethod) {
    throw std::runtime_error("Invalid method: putBoolean(Ljava/lang/String;Z)Landroid/content/SharedPreferences$Editor;");
  }

  _putIntMethod = _env.GetMethodID(editorClass, "putInt",
                                   "(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;");

  if (!_putIntMethod) {
    throw std::runtime_error("Invalid method: putInt(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;");
  }

  _applyMethod = _env.GetMethodID(editorClass, "apply", "()V");

  if (!_applyMethod) {
    throw std::runtime_error("Invalid method: apply()V");
  }
}

SharedPreferences::Editor& SharedPreferences::Editor::putBoolean(std::string key, bool value) {
  auto editor = _env.CallObjectMethod(_editor, _putBooleanMethod, _env.NewStringUTF(key.data()), value);

  if (!_env.IsSameObject(editor, _editor)) {
    throw std::runtime_error("Unexpected method result");
  }

  return *this;
}

SharedPreferences::Editor& SharedPreferences::Editor::putInt(std::string key, int32_t value) {
  auto editor = _env.CallObjectMethod(_editor, _putIntMethod, _env.NewStringUTF(key.data()), value);

  if (!_env.IsSameObject(editor, _editor)) {
    throw std::runtime_error("Unexpected method result");
  }

  return *this;
}

void SharedPreferences::Editor::apply() {
  _env.CallObjectMethod(_editor, _applyMethod);
}

}  // namespace linguine::carbonara
