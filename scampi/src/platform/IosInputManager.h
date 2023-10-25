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
      return CursorLocation { 0.0f, 0.0f };
    }

    [[nodiscard]] bool isSwipeDetected(Direction direction) const override {
      switch (direction) {
      case InputManager::Direction::Left:
        return _currentLeftSwipe;
      case InputManager::Direction::Right:
        return _currentRightSwipe;
      default:
        return false;
      }
    }

    void onTouchBegan(uint64_t id, float x, float y);

    void onTouchMoved(uint64_t id, float x, float y);

    void onTouchEnded(uint64_t id, float x, float y);

    void onTouchCanceled(uint64_t id, float x, float y);

    void onLeftSwipe();

    void onRightSwipe();

  private:
    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, Touch> _pending;

    bool _pendingLeftSwipe;
    bool _currentLeftSwipe;

    bool _pendingRightSwipe;
    bool _currentRightSwipe;
};

}
