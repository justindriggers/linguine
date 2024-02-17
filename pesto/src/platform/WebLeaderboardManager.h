#pragma once

#include <LeaderboardManager.h>

namespace linguine::pesto {

class WebLeaderboardManager : public LeaderboardManager {
  public:
    ~WebLeaderboardManager() override = default;

    bool isAvailable() override {
      return false;
    }

    void getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                       std::function<void(std::string reason)> onError) override {
      onError("Unsupported");
    }

    void submitScore(int32_t score,
                     std::function<void()> onSuccess,
                     std::function<void(std::string reason)> onError) override {
      onError("Unsupported");
    }
};

}  // namespace linguine::pesto
