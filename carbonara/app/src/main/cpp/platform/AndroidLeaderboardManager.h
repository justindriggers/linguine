#pragma once

#include <LeaderboardManager.h>

#include "jni/Leaderboards.h"

namespace linguine::carbonara {

class AndroidLeaderboardManager : public LeaderboardManager {
  public:
    explicit AndroidLeaderboardManager(android_app& app)
        : _leaderboards(app) {}

    ~AndroidLeaderboardManager() override = default;

    bool isAvailable() override;

    void getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                       std::function<void(std::string reason)> onError) override;

    void submitScore(int32_t score,
                     std::function<void()> onSuccess,
                     std::function<void(std::string reason)> onError) override;

  private:
    const std::string LeaderboardId = "CgkIw6Db0Y4KEAIQBA";

    Leaderboards _leaderboards;
};

}  // namespace linguine::carbonara
