#pragma once

#include <TimeManager.h>

namespace linguine::carbonara {

class AndroidTimeManager : public TimeManager {
  public:
    AndroidTimeManager() : TimeManager(currentTime()) {}

    ~AndroidTimeManager() override = default;

    [[nodiscard]] time_t currentTime() const override;

    [[nodiscard]] float durationInSeconds(time_t from, time_t to) const override;
};

}  // namespace linguine::carbonara
