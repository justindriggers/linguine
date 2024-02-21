#pragma once

#include <Logger.h>

namespace linguine::carbonara {

class AndroidLogger : public Logger {
  public:
    ~AndroidLogger() override = default;

    void log(const std::string& log) const override;
};

}  // namespace linguine::carbonara
