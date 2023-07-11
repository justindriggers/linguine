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

}  // namespace linguine::pesto
