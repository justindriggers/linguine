#pragma once

#include <input/InputManager.h>

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace linguine::carbonara {

class AndroidInputManager : public InputManager {
  public:
    explicit AndroidInputManager(android_app& app) : _app(app) {}

    ~AndroidInputManager() override = default;

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
      return false;
    }

    static void pollNativeEvents(android_app& app);

  private:
    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, Touch> _pending;

    android_app& _app;
};

}  // namespace linguine::carbonara
