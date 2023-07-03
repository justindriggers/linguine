#pragma once

#include <TimeManager.h>

namespace linguine::pesto {

class WebTimeManager : public TimeManager {
  public:
    WebTimeManager() : TimeManager(currentTime()) {}

    ~WebTimeManager() override = default;

    [[nodiscard]] time_t currentTime() const override;

    [[nodiscard]] float durationInSeconds(time_t from, time_t to) const override;
};

}
