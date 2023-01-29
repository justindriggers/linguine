#pragma once

#include <TimeManager.h>

namespace linguine::scampi {
  class IosTimeManager : public TimeManager {
    public:
      [[nodiscard]] time_t currentTime() const override;

      [[nodiscard]] float durationInSeconds(time_t from, time_t to) const override;
  };
}