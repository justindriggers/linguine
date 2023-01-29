#pragma once

#include <Logger.h>

namespace linguine::scampi {
  class NSLogger : public Logger {
    public:
      void log(const std::string& log) const override;

      template<class T>
      void log(T log) const {}
  };
}
