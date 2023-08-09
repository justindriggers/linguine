#pragma once

#include <input/InputManager.h>

#include <unordered_map>

namespace linguine::scampi {

class IosInputManager : public InputManager {
  public:
    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override;

    [[nodiscard]] float getSensitivity() const override;

    [[nodiscard]] bool isKeyPressed(Key key) const override {
      return false;
    }

    [[nodiscard]] CursorLocation getCursorLocation() const override {
      return CursorLocation { -1.0f, -1.0f };
    }

    void onTouchBegan(uint64_t id, float x, float y);

    void onTouchMoved(uint64_t id, float x, float y);

    void onTouchEnded(uint64_t id, float x, float y);

    void onTouchCanceled(uint64_t id, float x, float y);

  private:
    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, Touch> _pending;
};

}
