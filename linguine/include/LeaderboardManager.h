#pragma once

#include <string>

namespace linguine {

class LeaderboardManager {
  public:
    struct Record {
      std::string name;
      int32_t score;
      bool isPlayer;
    };

    virtual ~LeaderboardManager() = default;

    virtual bool isAvailable() = 0;

    virtual void getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                               std::function<void(std::string reason)> onError) = 0;

    virtual void submitScore(int32_t score,
                             std::function<void()> onSuccess,
                             std::function<void(std::string reason)> onError) = 0;
};

}  // namespace linguine
