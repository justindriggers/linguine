#pragma once

#include <TimeManager.h>

namespace linguine::alfredo {
  class MacTimeManager : public TimeManager {
    public:
      MacTimeManager() : TimeManager(currentTime()) {}

      [[nodiscard]] time_t currentTime() const override;

      [[nodiscard]] float durationInSeconds(time_t from, time_t to) const override;
  };
}