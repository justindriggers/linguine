#pragma once

#include <unordered_map>

namespace linguine {
  class InputManager {
    public:
      /**
       * @param x Where 0.0f is the left, and 1.0f is the right
       * @param y Where 0.0f is the bottom, and 1.0f is the top
       */
      struct Touch {
        float x;
        float y;
      };

      virtual void pollEvents() = 0;

      [[nodiscard]] virtual const std::unordered_map<uint64_t, Touch>& getTouches() const = 0;
  };
}