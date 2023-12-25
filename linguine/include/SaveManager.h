#pragma once

#include <cstdint>
#include <unordered_map>

namespace linguine {

class SaveManager {
  public:
    SaveManager() = default;

    virtual ~SaveManager() = default;

    void addPoints(int32_t points) {
      _points += points;
      save();
    }

    void removePoints(int32_t points) {
      _points -= points;
      save();
    }

    [[nodiscard]] uint8_t getRank(uint8_t upgradeId) const {
      return _upgradeRanks.at(upgradeId);
    }

    void increaseRank(uint8_t upgradeId) {
      _upgradeRanks[upgradeId]++;
      save();
    }

    [[nodiscard]] int32_t getPoints() const {
      return _points;
    }

  protected:
    int32_t _points{};
    std::unordered_map<uint8_t, uint8_t> _upgradeRanks;

    virtual void load() = 0;

    virtual void save() = 0;
};

}  // namespace linguine
