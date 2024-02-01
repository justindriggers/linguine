#pragma once

#include <array>
#include <cstdint>

namespace linguine {

class SaveManager {
  public:
    enum class Handedness {
      Left,
      Right
    };

    SaveManager() = default;

    virtual ~SaveManager() = default;

    void ensureLevel(uint8_t level) {
      if (_level < level) {
        _level = level;
        save();
      }
    }

    [[nodiscard]] uint8_t getCurrentLevel() const {
      return _level;
    }

    [[nodiscard]] bool isNewPlayer() const {
      return _level == 1;
    }

    void setMusicEnabled(bool enabled) {
      _isMusicEnabled = enabled;
      save();
    }

    [[nodiscard]] bool isMusicEnabled() const {
      return _isMusicEnabled;
    }

    void setSoundEffectsEnabled(bool enabled) {
      _isSoundEffectsEnabled = enabled;
      save();
    }

    [[nodiscard]] bool isSoundEffectsEnabled() const {
      return _isSoundEffectsEnabled;
    }

    void setScreenShakeEnabled(bool enabled) {
      _isScreenShakeEnabled = enabled;
      save();
    }

    [[nodiscard]] bool isScreenShakeEnabled() const {
      return _isScreenShakeEnabled;
    }

    void setHandedness(Handedness handedness) {
      _handedness = handedness;
      save();
    }

    [[nodiscard]] Handedness getHandedness() const {
      return _handedness;
    }

    void ensureStarsForLevel(uint8_t level, uint8_t stars) {
      auto index = level - 1;

      if (_stars[index] < stars) {
        _stars[index] = stars;
        save();
      }
    }

    [[nodiscard]] uint8_t getStarsForLevel(uint8_t level) {
      return _stars[level - 1];
    }

    void restart() {
      _level = 0;
      save();
    }

  protected:
    bool _isMusicEnabled{};
    bool _isSoundEffectsEnabled{};
    bool _isScreenShakeEnabled{};
    Handedness _handedness{};
    uint8_t _level = 1;
    std::array<uint8_t, 20> _stars{};

    virtual void load() = 0;

    virtual void save() = 0;
};

}  // namespace linguine
