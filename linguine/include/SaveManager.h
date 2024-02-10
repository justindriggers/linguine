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

    void setPoints(int32_t points) {
      _points = points;
      save();
    }

    [[nodiscard]] int32_t getPoints() const {
      return _points;
    }

    void setLives(uint8_t lives) {
      _lives = lives;
      save();
    }

    [[nodiscard]] uint8_t getLives() const {
      return _lives;
    }

    [[nodiscard]] bool isNewPlayer() const {
      return _points == 0;
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

    void restart() {
      _points = 0;
      _lives = 1;
      save();
    }

  protected:
    int32_t _points{};
    uint8_t _lives{};
    bool _isMusicEnabled{};
    bool _isSoundEffectsEnabled{};
    bool _isScreenShakeEnabled{};
    Handedness _handedness{};

    virtual void load() = 0;

    virtual void save() = 0;
};

}  // namespace linguine
