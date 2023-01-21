#pragma once

#include <ctime>

namespace linguine {
  class TimeManager {
    public:
      virtual time_t currentTime() const = 0;

      virtual float durationInSeconds(time_t from, time_t to) const = 0;
  };
}
