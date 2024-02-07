#pragma once

#include <array>

namespace linguine {

class LevelCurve {
public:
  static constexpr uint8_t MaxLevel = 20;

  static constexpr uint8_t getLevelForXp(int32_t xp) {
    auto level = 0;
    auto xpToLevel = 0;

    for (auto xpIncreasePerLevel : _xpIncreasePerLevel) {
      xpToLevel += xpIncreasePerLevel;

      if (xp >= xpToLevel) {
        xp -= xpToLevel;
        ++level;
      } else {
        break;
      }
    }

    return level;
  }

  static constexpr int32_t getXpPerLevel(uint8_t level) {
    auto result = std::array<int32_t, MaxLevel>();

    auto xpRequired = 0;

    for (auto i = 0; i < _xpIncreasePerLevel.size(); ++i) {
      xpRequired += _xpIncreasePerLevel[i];
      result[i] = xpRequired;
    }

    return result[level - 1];
  }

  static constexpr int32_t getXpForLevel(uint8_t level) {
    auto xp = 0;

    for (auto i = 1; i <= level; ++i) {
      xp += getXpPerLevel(i);
    }

    return xp;
  }

private:
  static constexpr std::array<int32_t, MaxLevel> _xpIncreasePerLevel = {
      0, 25, 25, 25, 50,
      25, 25, 25, 25, 50,
      25, 25, 25, 25, 50,
      25, 25, 25, 25, 50
  };
};

}  // namespace linguine
