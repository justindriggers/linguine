#pragma once

#include <Logger.h>

namespace linguine::pesto {

class WebLogger : public Logger {
  public:
    ~WebLogger() override = default;

    void log(const std::string& log) const override;
};

}  // namespace linguine::pesto
