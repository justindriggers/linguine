#pragma once

#include <string>

namespace linguine {
  class Logger {
    public:
      virtual void log(const std::string& log) const = 0;
  };
}
