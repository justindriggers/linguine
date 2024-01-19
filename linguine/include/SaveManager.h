#pragma once

#include <cstdint>

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

    [[nodiscard]] int32_t getPoints() const {
      return _points;
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

    void restart() {
      _points = 0;
      save();
    }

  protected:
    int32_t _points{};
    bool _isMusicEnabled{};
    bool _isSoundEffectsEnabled{};
    bool _isScreenShakeEnabled{};

    virtual void load() = 0;

    virtual void save() = 0;
};

}  // namespace linguine
