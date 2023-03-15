#pragma once

#include <ctime>

namespace linguine {

class TimeManager {
  public:
    [[nodiscard]] virtual time_t currentTime() const = 0;

    [[nodiscard]] virtual float durationInSeconds(time_t from, time_t to) const = 0;

    [[nodiscard]] float getFixedTimeStep() const {
      return _fixedDeltaTime;
    }

  private:
    constexpr static float _fixedDeltaTime = 0.02f;
};

}  // namespace linguine

