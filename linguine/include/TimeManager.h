#pragma once

#include <ctime>

namespace linguine {

class TimeManager {
  public:
    explicit TimeManager(time_t startTime)
        : _startTime(startTime),
          _lastTickTime(startTime),
          _currentTickTime(startTime) {}

    virtual ~TimeManager() = default;

    [[nodiscard]] virtual time_t currentTime() const = 0;

    [[nodiscard]] virtual float durationInSeconds(time_t from, time_t to) const = 0;

    [[nodiscard]] inline float scaledDuration(time_t from, time_t to) const {
      return durationInSeconds(from, to) * _timeScale;
    }

    [[nodiscard]] inline float runtimeInSeconds() const {
      return durationInSeconds(_startTime, currentTime());
    }

    [[nodiscard]] inline float getTimeStep() const {
      return scaledDuration(_lastTickTime, _currentTickTime);
    }

    [[nodiscard]] inline float getFixedTimeStep() const {
      return _fixedDeltaTime;
    }

    inline void pause() {
      setTimeScale(0.0f);
    }

    inline void resume() {
      setTimeScale(1.0f);
      _lastTickTime = currentTime();
      _currentTickTime = _lastTickTime;
    }

    inline void reset() {
      _lastTickTime = currentTime();
      _currentTickTime = _lastTickTime;
      _accumulator = 0.0f;
    }

    inline float tick() {
      _lastTickTime = _currentTickTime;
      _currentTickTime = currentTime();

      auto deltaTime = getTimeStep();
      _accumulator += deltaTime;

      return deltaTime;
    }

    [[nodiscard]] inline bool fixedTick() {
      if (_accumulator >= _fixedDeltaTime) {
        _accumulator -= _fixedDeltaTime;
        return true;
      }

      return false;
    }

    inline void setTimeScale(float timeScale) {
      _timeScale = timeScale;
    }

  private:
    constexpr static float _fixedDeltaTime = 0.02f;

    float _accumulator = 0.0f;
    float _timeScale = 1.0f;

    time_t _startTime;
    time_t _lastTickTime;
    time_t _currentTickTime;
};

}  // namespace linguine
