#pragma once

#include <LeaderboardManager.h>

#include <random>

namespace linguine::alfredo {

class MacLeaderboardManager : public LeaderboardManager {
  public:
    ~MacLeaderboardManager() override = default;

    bool isAvailable() override {
      return true;
    }

    void getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                       std::function<void(std::string reason)> onError) override;

    void submitScore(int32_t score,
                     std::function<void()> onSuccess,
                     std::function<void(std::string reason)> onError) override;

  private:
    std::random_device _random;
};

}  // namespace linguine::alfredo
