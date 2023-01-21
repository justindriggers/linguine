#pragma once

namespace linguine {
  class InputManager {
    public:
      virtual void pollEvents() = 0;
  };
}