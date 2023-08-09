#include "GestureRecognitionSystem.h"

#include <glm/geometric.hpp>

#include "components/Hovered.h"
#include "components/LongPressed.h"
#include "components/Tapped.h"

namespace linguine {

void GestureRecognitionSystem::update(float deltaTime) {
  findEntities<LongPressed>()->each([](Entity& entity) {
    entity.remove<LongPressed>();
  });

  findEntities<Tapped>()->each([](Entity& entity) {
    entity.remove<Tapped>();
  });

  findEntities<Hovered>()->each([](Entity& entity) {
    entity.remove<Hovered>();
  });

  const auto cursorLocation = _inputManager.getCursorLocation();
  const auto entityId = _renderer.getEntityIdAt(cursorLocation.x, cursorLocation.y);

  if (entityId) {
    auto entity = getEntityById(*entityId);
    entity->add<Hovered>();
  }

  for (auto& touch : _inputManager.getTouches()) {
    switch (touch.second.state) {
      case InputManager::Down:
        onDownEvent(touch.first, touch.second);
        break;
      case InputManager::Hold:
        onHoldEvent(touch.first, touch.second);
        break;
      case InputManager::Up:
        onUpEvent(touch.first, touch.second);
        break;
    }
  }
}

void GestureRecognitionSystem::onDownEvent(uint64_t id, const InputManager::Touch& touch) {
  const auto entityId = _renderer.getEntityIdAt(touch.x, touch.y);

  auto position = glm::vec2(touch.x, touch.y);
  _gestureStates.insert({id, { _timeManager.currentTime(), entityId, position, position }});
}

void GestureRecognitionSystem::onHoldEvent(uint64_t id, const InputManager::Touch& touch) {
  const auto& it = _gestureStates.find(id);

  if (it != _gestureStates.end()) {
    auto& gestureState = it->second;
    auto newLocation = glm::vec2(touch.x, touch.y);

    const auto distance = glm::length(newLocation - gestureState.startLocation);
    const auto duration = _timeManager.durationInSeconds(gestureState.startTime, _timeManager.currentTime());

    if (duration >= _longPressThreshold) {
      const auto entityId = _renderer.getEntityIdAt(touch.x, touch.y);

      if (entityId && entityId == gestureState.startEntityId) {
        auto entity = getEntityById(entityId.value());
        entity->add<LongPressed>();
      }

      _gestureStates.erase(it->first);
    }

    gestureState.lastLocation = newLocation;
  }
}

void GestureRecognitionSystem::onUpEvent(uint64_t id, const InputManager::Touch& touch) {
  const auto& it = _gestureStates.find(id);

  if (it != _gestureStates.end()) {
    auto& gestureState = it->second;

    if (gestureState.startEntityId.has_value()) {
      const auto distance = glm::length(gestureState.startLocation - glm::vec2(touch.x, touch.y));
      const auto duration = _timeManager.durationInSeconds(gestureState.startTime, _timeManager.currentTime());

      if (distance < _tapDistanceThreshold && duration < _longPressThreshold) {
        auto entity = getEntityById(gestureState.startEntityId.value());
        entity->add<Tapped>();
      }
    }

    _gestureStates.erase(id);
  }
}

}  // namespace linguine
