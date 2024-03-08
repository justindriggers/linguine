#include "AndroidLeaderboardManager.h"

namespace linguine::carbonara {

bool AndroidLeaderboardManager::isAvailable() {
  return _leaderboards.isAuthenticated() && !_leaderboards.getPlayerId().empty();
}

void AndroidLeaderboardManager::getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                                              std::function<void(std::string reason)> onError) {
  auto playerId = _leaderboards.getPlayerId();

  _leaderboards.loadTopScores(LeaderboardId, Leaderboards::TimeSpan::AllTime,
                              Leaderboards::Collection::Public, 10,
                              [onSuccess, playerId](const std::vector<Leaderboards::ScoreEntry>& scoreEntries) {
                                auto results = std::vector<Record>(scoreEntries.size());

                                for (auto i = 0; i < scoreEntries.size(); ++i) {
                                  const auto& entry = scoreEntries[i];
                                  results[i] = { entry.displayName, static_cast<int32_t>(entry.score), entry.playerId == playerId };
                                }

                                onSuccess(results);
                              }, [onError]() {
                                onError("Error");
                              });
}

void AndroidLeaderboardManager::submitScore(int32_t score, std::function<void()> onSuccess,
                                            std::function<void(std::string reason)> onError) {
  _leaderboards.submitScore(LeaderboardId, score, onSuccess, [onError]() {
    onError("Error");
  });
}

}  // namespace linguine::carbonara
