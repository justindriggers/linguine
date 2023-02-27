#pragma once

#include <InputManager.h>

namespace linguine::alfredo {

class MacInputManager : public InputManager {
  public:
    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override;

  private:
    std::unordered_map<uint64_t, Touch> _active;
};

}  // namespace linguine::alfredo
