#include "WebInputManager.h"

#include <emscripten/html5.h>

namespace linguine::pesto {

WebInputManager::WebInputManager(const Viewport& viewport) : _viewport(viewport) {
  emscripten_set_mousedown_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<WebInputManager*>(userData)->onMouseDown(mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
    return true;
  });

  emscripten_set_mouseup_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<WebInputManager*>(userData)->onMouseUp(mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
    return true;
  });

  emscripten_set_mousemove_callback("canvas", this, false, [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) -> EM_BOOL {
    static_cast<WebInputManager*>(userData)->onMouseMoved(mouseEvent->button, mouseEvent->targetX, mouseEvent->targetY);
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

void WebInputManager::onMouseMoved(unsigned short button, long x, long y) {
  auto touch = Touch {
      static_cast<float>(x) / _viewport.getWidth(),
      1.0f - static_cast<float>(y) / _viewport.getHeight(),
      Hold
  };
  _pending.insert({button, touch});
}

void WebInputManager::onKeyDown(const std::string& key) {
  if (key == "w" || key == "W") {
    _keyStates[W] = true;
  } else if (key == "a" || key == "A") {
    _keyStates[A] = true;
  } else if (key == "s" || key == "S") {
    _keyStates[S] = true;
  } else if (key == "d" || key == "D") {
    _keyStates[D] = true;
  }
}

void WebInputManager::onKeyUp(const std::string& key) {
  if (key == "w" || key == "W") {
    _keyStates[W] = false;
  } else if (key == "a" || key == "A") {
    _keyStates[A] = false;
  } else if (key == "s" || key == "S") {
    _keyStates[S] = false;
  } else if (key == "d" || key == "D") {
    _keyStates[D] = false;
  }
}

}  // namespace linguine::pesto
