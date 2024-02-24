#pragma once

#include <input/InputManager.h>

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include <renderer/Viewport.h>

#include "jni/SwipeListener.h"

namespace linguine::carbonara {

class AndroidInputManager : public InputManager, SwipeListener {
  public:
    AndroidInputManager(android_app& app, const Viewport& viewport);

    ~AndroidInputManager() override;

    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override {
      return _active;
    }

    [[nodiscard]] float getSensitivity() const override {
      return 1.0f;
    }

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

    void onLeftSwipe() override;

    void onRightSwipe() override;

    static void pollNativeEvents(android_app& app);

  private:
    std::unordered_map<uint64_t, Touch> _active;

    android_app& _app;
    const Viewport& _viewport;

    bool _pendingLeftSwipe = false;
    bool _currentLeftSwipe = false;

    bool _pendingRightSwipe = false;
    bool _currentRightSwipe = false;
};

}  // namespace linguine::carbonara
