#pragma once

#include <unordered_map>

namespace linguine {
  class InputManager {
    public:
      struct Touch {
        float x;
        float y;
      };

      virtual void pollEvents() = 0;

      [[nodiscard]] virtual const std::unordered_map<uint64_t, Touch>& getTouches() const = 0;
  };
}