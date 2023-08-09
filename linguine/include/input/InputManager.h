#pragma once

#include <unordered_map>

#include "Key.h"

namespace linguine {

class InputManager {
  public:
    enum TouchState {
      Down,
      Hold,
      Up
    };

    /**
     * @param x Where 0.0f is the left, and 1.0f is the right
     * @param y Where 0.0f is the bottom, and 1.0f is the top
     */
    struct Touch {
      float x;
      float y;
      TouchState state;
    };

    /**
     * @param x Where 0.0f is the left, and 1.0f is the right
     * @param y Where 0.0f is the bottom, and 1.0f is the top
     */
    struct CursorLocation {
      float x;
      float y;
    };

    virtual ~InputManager() = default;

    virtual void pollEvents() = 0;

    [[nodiscard]] virtual const std::unordered_map<uint64_t, Touch>& getTouches() const = 0;

    [[nodiscard]] virtual float getSensitivity() const = 0;

    [[nodiscard]] virtual bool isKeyPressed(Key key) const = 0;

    [[nodiscard]] virtual CursorLocation getCursorLocation() const = 0;
};

}  // namespace linguine
