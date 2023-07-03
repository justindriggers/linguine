#pragma once

#include <InputManager.h>

namespace linguine::pesto {

class WebInputManager : public InputManager {
  public:
    ~WebInputManager() override = default;

    void pollEvents() override {}

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override {
      return _touches;
    }

    [[nodiscard]] float getSensitivity() const override {
      return 1.0f;
    }

  private:
    std::unordered_map<uint64_t, Touch> _touches = {};
};

}  // namespace linguine::pesto
