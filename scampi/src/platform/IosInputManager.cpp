#include "IosInputManager.h"

namespace linguine::scampi {

void IosInputManager::pollEvents() {
  for (const auto &entry : _pending) {
    if (!entry.second.isActive) {
      _active.erase(entry.first);
    } else {
      _active[entry.first] = Touch { entry.second.x, entry.second.y };
    }
  }

  _pending.clear();
}

const std::unordered_map<uint64_t, InputManager::Touch>& IosInputManager::getTouches() const {
  return _active;
}

void IosInputManager::enqueue(uint64_t id, const TouchEvent& touchEvent) {
  _pending[id] = touchEvent;
}

}  // namespace linguine::scampi
