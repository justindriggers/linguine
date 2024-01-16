#include "WebInputManager.h"

#include <emscripten/html5.h>

namespace linguine::pesto {

WebInputManager::WebInputManager(const Viewport& viewport, AudioManager& audioManager) : _viewport(viewport) {
  emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &audioManager, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<AudioManager*>(userData)->resume();
    return true;
  });

  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &audioManager, false, [](int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) -> EM_BOOL {
    static_cast<AudioManager*>(userData)->resume();
    return true;
  });

  emscripten_set_mousedown_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<WebInputManager*>(userData)->onMouseDown(mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
    return true;
  });

  emscripten_set_mouseup_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<WebInputManager*>(userData)->onMouseUp(mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
    return true;
  });

  emscripten_set_mousemove_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    auto inputManager = static_cast<WebInputManager*>(userData);

    if (mouseEvent->buttons) {
      if (mouseEvent->buttons & 1) {
        inputManager->onMouseDragged(0, mouseEvent->targetX, mouseEvent->targetY);
      }

      if (mouseEvent->buttons & 2) {
        inputManager->onMouseDragged(1, mouseEvent->targetX, mouseEvent->targetY);
      }

      if (mouseEvent->buttons & 4) {
        inputManager->onMouseDragged(2, mouseEvent->targetX, mouseEvent->targetY);
      }

      if (mouseEvent->buttons & 8) {
        inputManager->onMouseDragged(3, mouseEvent->targetX, mouseEvent->targetY);
      }

      if (mouseEvent->buttons & 16) {
        inputManager->onMouseDragged(4, mouseEvent->targetX, mouseEvent->targetY);
      }
    }

    inputManager->onMouseMoved(mouseEvent->targetX, mouseEvent->targetY);
    return true;
  });

  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, false, [](int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) -> EM_BOOL {
    auto key = std::string(keyEvent->key);
    static_cast<WebInputManager*>(userData)->onKeyDown(key);
    return true;
  });

  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, false, [](int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) -> EM_BOOL {
    auto key = std::string(keyEvent->key);
    static_cast<WebInputManager*>(userData)->onKeyUp(key);
    return true;
  });
}

void WebInputManager::pollEvents() {
  auto toRemove = std::vector<uint64_t>();

  for (auto& entry : _active) {
    switch (entry.second.state) {
    case TouchState::Down:
      entry.second.state = Hold;
      break;
    case TouchState::Up:
      toRemove.push_back(entry.first);
      break;
    default:
      break;
    }
  }

  for (const auto id : toRemove) {
    _active.erase(id);
  }

  for (const auto& entry : _pending) {
    if (entry.second.state == Hold
        && _active.find(entry.first) == _active.end()) {
      continue;
    }

    auto& active = _active[entry.first];
    active.x = entry.second.x;
    active.y = entry.second.y;
    active.state = entry.second.state;
  }

  _pending.clear();
}

bool WebInputManager::isKeyPressed(Key key) const {
  return _keyStates[key];
}

InputManager::CursorLocation WebInputManager::getCursorLocation() const {
  return _cursorLocation;
}

void WebInputManager::onMouseDown(unsigned short button, long x, long y) {
  auto touch = Touch {
      static_cast<float>(x) / _viewport.getWidth(),
      1.0f - static_cast<float>(y) / _viewport.getHeight(),
      Down
  };
  _pending.insert({button, touch});
}

void WebInputManager::onMouseUp(unsigned short button, long x, long y) {
  auto touch = Touch {
      static_cast<float>(x) / _viewport.getWidth(),
      1.0f - static_cast<float>(y) / _viewport.getHeight(),
      Up
  };
  _pending.insert({button, touch});
}

void WebInputManager::onMouseDragged(unsigned short button, long x, long y) {
  auto touch = Touch {
      static_cast<float>(x) / _viewport.getWidth(),
      1.0f - static_cast<float>(y) / _viewport.getHeight(),
      Hold
  };
  _pending.insert({button, touch});
}

void WebInputManager::onMouseMoved(long x, long y) {
  if (x < 0 || x > _viewport.getWidth()
      || y < 0 || y > _viewport.getHeight()) {
    return;
  }

  _cursorLocation.x = static_cast<float>(x) / _viewport.getWidth();
  _cursorLocation.y = 1.0f - static_cast<float>(y + 1) / _viewport.getHeight();
}

void WebInputManager::onKeyDown(const std::string& key) {
  if (key == "q" || key == "Q") {
    _keyStates[Q] = true;
  } else if (key == "w" || key == "W") {
    _keyStates[W] = true;
  } else if (key == "e" || key == "E") {
    _keyStates[E] = true;
  } else if (key == "r" || key == "R") {
    _keyStates[R] = true;
  } else if (key == "a" || key == "A") {
    _keyStates[A] = true;
  } else if (key == "s" || key == "S") {
    _keyStates[S] = true;
  } else if (key == "d" || key == "D") {
    _keyStates[D] = true;
  } else if (key == "f" || key == "F") {
    _keyStates[F] = true;
  } else if (key == "c" || key == "C") {
    _keyStates[C] = true;
  }
}

void WebInputManager::onKeyUp(const std::string& key) {
  if (key == "q" || key == "Q") {
    _keyStates[Q] = false;
  } else if (key == "w" || key == "W") {
    _keyStates[W] = false;
  } else if (key == "e" || key == "E") {
    _keyStates[E] = false;
  } else if (key == "r" || key == "R") {
    _keyStates[R] = false;
  } else if (key == "a" || key == "A") {
    _keyStates[A] = false;
  } else if (key == "s" || key == "S") {
    _keyStates[S] = false;
  } else if (key == "d" || key == "D") {
    _keyStates[D] = false;
  } else if (key == "f" || key == "F") {
    _keyStates[F] = false;
  } else if (key == "c" || key == "C") {
    _keyStates[C] = false;
  }
}

}  // namespace linguine::pesto
