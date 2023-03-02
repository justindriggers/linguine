#include "IosInputManager.h"

#include <vector>

namespace linguine::scampi {

void IosInputManager::pollEvents() {
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
    auto& active = _active[entry.first];
    active.x = entry.second.x;
    active.y = entry.second.y;
    active.state = entry.second.state;
  }

  _pending.clear();
}

const std::unordered_map<uint64_t, InputManager::Touch>& IosInputManager::getTouches() const {
  return _active;
}

float IosInputManager::getSensitivity() const {
  return 300.0f;
}

void IosInputManager::onTouchBegan(uint64_t id, float x, float y) {
  _pending.insert({id, { x, y, Down }});
}

void IosInputManager::onTouchMoved(uint64_t id, float x, float y) {
  _pending.insert({id, { x, y, Hold }});
}

void IosInputManager::onTouchEnded(uint64_t id, float x, float y) {
  _pending.insert({id, { x, y, Up }});
}

void IosInputManager::onTouchCanceled(uint64_t id, float x, float y) {
  _pending.insert({id, { x, y, Up }});
}

}  // namespace linguine::scampi
