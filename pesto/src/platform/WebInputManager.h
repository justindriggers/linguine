#pragma once

#include <InputManager.h>

#include <renderer/Viewport.h>

namespace linguine::pesto {

class WebInputManager : public InputManager {
  public:
    WebInputManager(const Viewport& viewport);

    ~WebInputManager() override = default;

    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override {
      return _active;
    }

    [[nodiscard]] float getSensitivity() const override {
      return 1.0f;
    }

  private:
    const Viewport& _viewport;

    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, Touch> _pending;

    void onMouseDown(unsigned short button, long x, long y);

    void onMouseUp(unsigned short button, long x, long y);

    void onMouseMoved(unsigned short button, long x, long y);
};

}  // namespace linguine::pesto
