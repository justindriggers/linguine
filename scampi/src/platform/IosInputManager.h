#pragma once

#include <InputManager.h>

namespace linguine::scampi {
  class IosInputManager : public InputManager {
    public:
      void pollEvents() override;
  };
}
