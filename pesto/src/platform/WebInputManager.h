#pragma once

#include <input/InputManager.h>

#include <bitset>

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

    [[nodiscard]] bool isKeyPressed(Key key) const override;

    [[nodiscard]] CursorLocation getCursorLocation() const override;

    [[nodiscard]] bool isSwipeDetected(Direction direction) const override {
      return false;
    }

  private:
    const Viewport& _viewport;

    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, Touch> _pending;
    std::bitset<Key::MAX> _keyStates;
    CursorLocation _cursorLocation{};

    void onMouseDown(unsigned short button, long x, long y);

    void onMouseUp(unsigned short button, long x, long y);

    void onMouseDragged(unsigned short button, long x, long y);

    void onMouseMoved(long x, long y);

    void onKeyDown(const std::string& key);

    void onKeyUp(const std::string& key);
};

}  // namespace linguine::pesto
