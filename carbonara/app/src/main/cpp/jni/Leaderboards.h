#pragma once

#include <functional>
#include <string>
#include <vector>

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace linguine::carbonara {

class Leaderboards {
  public:
    enum Collection {
      Public = 0,
      Friends = 3
    };

    enum TimeSpan {
      Daily = 0,
      Weekly = 1,
      AllTime = 2
    };

    struct ScoreEntry {
      std::string playerId;
      std::string displayName;
      int64_t score;
    };

    explicit Leaderboards(android_app& app);

    ~Leaderboards();

    [[nodiscard]] bool isAuthenticated() const;

    [[nodiscard]] std::string getPlayerId() const;

    void loadTopScores(std::string leaderboardId, TimeSpan span,
                       Collection leaderboardCollection, int32_t maxResults,
                       std::function<void(const std::vector<ScoreEntry>& scoreEntries)> onSuccess,
                       std::function<void()> onError) const;

    void submitScore(std::string leaderboardId, int64_t score,
                     std::function<void()> onSuccess,
                     std::function<void()> onError) const;

  private:
    android_app& _app;
    JNIEnv* _env{};

    jobject _leaderboards{};
    jmethodID _isAuthenticatedMethod{};
    jmethodID _getPlayerIdMethod{};
    jmethodID _loadTopScoresMethod{};
    jmethodID _submitScoreMethod{};
};

}  // namespace linguine::carbonara
