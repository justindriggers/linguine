#pragma once

#include <string>

namespace linguine {

class Logger {
  public:
    virtual ~Logger() = default;

    virtual void log(const std::string& log) const = 0;
};

}  // namespace linguine
