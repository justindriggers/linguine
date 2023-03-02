#pragma once

#include <InputManager.h>

#include <unordered_map>

namespace linguine::alfredo {

class MacInputManager : public InputManager {
  public:
    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override;

    [[nodiscard]] float getSensitivity() const override;

  private:
    std::unordered_map<uint64_t, Touch> _active;
};

}  // namespace linguine::alfredo
