#pragma once

#include <InputManager.h>

#include <unordered_map>

namespace linguine::scampi {

class IosInputManager : public InputManager {
  public:
    struct TouchEvent {
      float x;
      float y;
      bool isActive;
    };

    void pollEvents() override;

    [[nodiscard]] const std::unordered_map<uint64_t, Touch>& getTouches() const override;

    void enqueue(uint64_t id, const TouchEvent& touchEvent);

  private:
    std::unordered_map<uint64_t, Touch> _active;
    std::unordered_map<uint64_t, TouchEvent> _pending;
};

}
