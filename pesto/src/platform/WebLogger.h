#pragma once

#include <Logger.h>

namespace linguine::pesto {

class WebLogger : public Logger {
  public:
    ~WebLogger() override = default;

    void log(const std::string& log) const override;

    template<class T>
    void log(T log) const {}
};

}  // namespace linguine::pesto
