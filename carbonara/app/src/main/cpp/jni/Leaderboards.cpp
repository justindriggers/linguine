#include "Leaderboards.h"

#include <jni.h>

namespace linguine::carbonara {

Leaderboards::Leaderboards(android_app& app) : _app(app) {
  auto result = _app.activity->vm->GetEnv(reinterpret_cast<void**>(&_env), JNI_VERSION_1_6);

  if (result == JNI_EDETACHED) {
    result = _app.activity->vm->AttachCurrentThread(&_env, nullptr);
  }

  if (result != JNI_OK) {
    throw std::runtime_error("Error retrieving JNI environment");
  }

  auto gameActivityClass = _env->GetObjectClass(_app.activity->javaGameActivity);
  auto getLeaderboardsMethod = _env->GetMethodID(gameActivityClass, "getLeaderboards",
                                                      "()Lcom/justindriggers/carbonara/leaderboards/Leaderboards;");

  if (!getLeaderboardsMethod) {
    throw std::runtime_error("Invalid method: getLeaderboards()Lcom/justindriggers/carbonara/leaderboards/Leaderboards;");
  }

  _leaderboards = _env->NewGlobalRef(_env->CallObjectMethod(_app.activity->javaGameActivity, getLeaderboardsMethod));

  auto leaderboardsClass = _env->GetObjectClass(_leaderboards);

  _isAuthenticatedMethod = _env->GetMethodID(leaderboardsClass, "isAuthenticated", "()Z");

  if (!_isAuthenticatedMethod) {
    throw std::runtime_error("Invalid method: isAuthenticated()Z");
  }

  _getPlayerIdMethod = _env->GetMethodID(leaderboardsClass, "getPlayerId", "()Ljava/lang/String;");

  if (!_getPlayerIdMethod) {
    throw std::runtime_error("Invalid method: getPlayerId()Ljava/lang/String;");
  }

  _loadTopScoresMethod = _env->GetMethodID(leaderboardsClass, "loadTopScores", "(Ljava/lang/String;IIIJ)V");

  if (!_loadTopScoresMethod) {
    throw std::runtime_error("Invalid method: loadTopScores(Ljava/lang/String;IIIJ)V");
  }

  _submitScoreMethod = _env->GetMethodID(leaderboardsClass, "submitScore", "(Ljava/lang/String;JJ)V");

  if (!_submitScoreMethod) {
    throw std::runtime_error("Invalid method: submitScore(Ljava/lang/String;JJ)V");
  }
}

Leaderboards::~Leaderboards() {
  _env->DeleteGlobalRef(_leaderboards);
};

bool Leaderboards::isAuthenticated() const {
  return _env->CallBooleanMethod(_leaderboards, _isAuthenticatedMethod);
}

std::string Leaderboards::getPlayerId() const {
  auto playerIdObj = reinterpret_cast<jstring>(_env->CallObjectMethod(_leaderboards, _getPlayerIdMethod));
  auto playerId = _env->GetStringUTFChars(playerIdObj, nullptr);

  auto result = std::string(playerId);
  _env->ReleaseStringUTFChars(playerIdObj, playerId);

  return result;
}

struct LoadTopScoresCallbacks {
  std::function<void(std::vector<Leaderboards::ScoreEntry> scoreEntries)> onSuccess;
  std::function<void()> onError;
};

void Leaderboards::loadTopScores(std::string leaderboardId, TimeSpan span,
                                 Collection leaderboardCollection, int maxResults,
                                 std::function<void(const std::vector<ScoreEntry>& scoreEntries)> onSuccess,
                                 std::function<void()> onError) const {
  auto callbacks = new LoadTopScoresCallbacks { std::move(onSuccess), std::move(onError) };

  _env->CallVoidMethod(_leaderboards, _loadTopScoresMethod,
                       _env->NewStringUTF(leaderboardId.data()), span,
                       leaderboardCollection, maxResults,
                       reinterpret_cast<jlong>(callbacks));
}

struct SubmitScoreCallbacks {
  std::function<void()> onSuccess;
  std::function<void()> onError;
};

void Leaderboards::submitScore(std::string leaderboardId, int64_t score,
                               std::function<void()> onSuccess,
                               std::function<void()> onError) const {
  auto callbacks = new SubmitScoreCallbacks { std::move(onSuccess), std::move(onError) };

  _env->CallVoidMethod(_leaderboards, _submitScoreMethod,
                       _env->NewStringUTF(leaderboardId.data()), score,
                       reinterpret_cast<jlong>(callbacks));
}

extern "C" {

jclass _scoreEntryClass{};
jfieldID _playerIdField{};
jfieldID _displayNameField{};
jfieldID _scoreField{};

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_leaderboards_LoadTopScoresCompletionListener_onSuccess(JNIEnv* env, jobject thiz, jobjectArray scoreEntries, jlong userData) {
  auto count = env->GetArrayLength(scoreEntries);

  auto results = std::vector<Leaderboards::ScoreEntry>(count);

  for (auto i = 0; i < count; ++i) {
    auto entry = env->GetObjectArrayElement(scoreEntries, i);

    if (!_scoreEntryClass) {
      _scoreEntryClass = env->GetObjectClass(entry);

      _playerIdField = env->GetFieldID(_scoreEntryClass, "playerId", "Ljava/lang/String;");

      if (!_playerIdField) {
        throw std::runtime_error("Invalid field: Ljava/lang/String; playerId");
      }

      _displayNameField = env->GetFieldID(_scoreEntryClass, "displayName", "Ljava/lang/String;");

      if (!_displayNameField) {
        throw std::runtime_error("Invalid field: Ljava/lang/String; displayName");
      }

      _scoreField = env->GetFieldID(_scoreEntryClass, "score", "J");

      if (!_scoreField) {
        throw std::runtime_error("Invalid field: J score");
      }
    }

    auto playerIdObj = reinterpret_cast<jstring>(env->GetObjectField(entry, _playerIdField));
    auto playerId = env->GetStringUTFChars(playerIdObj, nullptr);

    auto displayNameObj = reinterpret_cast<jstring>(env->GetObjectField(entry, _displayNameField));
    auto displayName = env->GetStringUTFChars(displayNameObj, nullptr);

    auto score = env->GetLongField(entry, _scoreField);

    results[i] = { playerId, displayName, score };

    env->ReleaseStringUTFChars(playerIdObj, playerId);
    env->ReleaseStringUTFChars(displayNameObj, displayName);
  }

  auto callbacks = reinterpret_cast<LoadTopScoresCallbacks*>(userData);
  callbacks->onSuccess(results);

  delete callbacks;
}

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_leaderboards_LoadTopScoresCompletionListener_onFailure(JNIEnv* env, jobject thiz, jlong userData) {
  auto callbacks = reinterpret_cast<LoadTopScoresCallbacks*>(userData);
  callbacks->onError();

  delete callbacks;
}

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_leaderboards_SubmitScoreCompletionListener_onSuccess(JNIEnv* env, jobject thiz, jlong userData) {
  auto callbacks = reinterpret_cast<SubmitScoreCallbacks*>(userData);
  callbacks->onSuccess();

  delete callbacks;
}

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_leaderboards_SubmitScoreCompletionListener_onFailure(JNIEnv* env, jobject thiz, jlong userData) {
  auto callbacks = reinterpret_cast<SubmitScoreCallbacks*>(userData);
  callbacks->onError();

  delete callbacks;
}

}

}  // linguine::carbonara
