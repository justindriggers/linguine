#pragma once

#include <ctime>

namespace linguine {

class TimeManager {
  public:
    explicit TimeManager(time_t startTime) : _startTime(startTime) {}

    virtual ~TimeManager() = default;

    [[nodiscard]] virtual time_t currentTime() const = 0;

    [[nodiscard]] virtual float durationInSeconds(time_t from, time_t to) const = 0;

    [[nodiscard]] float runtimeInSeconds() const {
      return durationInSeconds(_startTime, currentTime());
    }

    [[nodiscard]] float getFixedTimeStep() const {
      return _fixedDeltaTime;
    }

  private:
    constexpr static float _fixedDeltaTime = 0.02f;

    time_t _startTime;
};

}  // namespace linguine
