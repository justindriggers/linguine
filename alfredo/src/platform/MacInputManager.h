#pragma once

#include <InputManager.h>

namespace linguine::alfredo {
  class MacInputManager : public InputManager {
    public:
      void pollEvents() override;
  };
}
