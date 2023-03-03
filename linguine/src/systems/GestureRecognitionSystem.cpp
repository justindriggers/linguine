#include "GestureRecognitionSystem.h"

#include <glm/geometric.hpp>

#include "components/CameraFixture.h"
#include "components/LongPressed.h"
#include "components/Tapped.h"
#include "components/Transform.h"

namespace linguine {

void GestureRecognitionSystem::update(float deltaTime) {
  findEntities<LongPressed>()->each([](Entity& entity) {
    entity.remove<LongPressed>();
  });

  findEntities<Tapped>()->each([](Entity& entity) {
    entity.remove<Tapped>();
  });

  for (auto& touch : _inputManager.getTouches()) {
    switch (touch.second.state) {
      case InputManager::Down:
        onDownEvent(touch.first, touch.second);
        break;
      case InputManager::Hold:
        onHoldEvent(touch.first, touch.second, deltaTime);
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

void GestureRecognitionSystem::onHoldEvent(uint64_t id, const InputManager::Touch& touch, float deltaTime) {
  const auto& it = _gestureStates.find(id);

  if (it != _gestureStates.end()) {
    auto& gestureState = it->second;
    auto newLocation = glm::vec2(touch.x, touch.y);

    if (_currentDragId == id
        || (!_currentDragId.has_value() && !gestureState.startEntityId.has_value())) {
      _currentDragId = id;
      auto direction = gestureState.lastLocation - newLocation;

      if (glm::length(direction) > 0.0f) {
        updateCamera(direction, deltaTime);
      }
    } else {
      const auto distance = glm::length(newLocation - gestureState.startLocation);
      const auto duration = _timeManager.durationInSeconds(gestureState.startTime, _timeManager.currentTime());

      if (!_currentDragId.has_value() && distance >= _dragThreshold) {
        _currentDragId = id;
        auto direction = gestureState.lastLocation - newLocation;

        if (glm::length(direction) > 0.0f) {
          updateCamera(direction, deltaTime);
        }
      } else if (duration >= _longPressThreshold) {
        const auto entityId = _renderer.getEntityIdAt(touch.x, touch.y);

        if (entityId && entityId == gestureState.startEntityId) {
          auto entity = getEntityById(entityId.value());
          entity->add<LongPressed>();
        }

        _gestureStates.erase(it->first);
      }
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

      if (distance < _dragThreshold && duration < _longPressThreshold) {
        auto entity = getEntityById(gestureState.startEntityId.value());
        entity->add<Tapped>();
      }
    }

    if (_currentDragId == id) {
      _currentDragId = {};
    }

    _gestureStates.erase(id);
  }
}

void GestureRecognitionSystem::updateCamera(glm::vec2 direction, float deltaTime) {
  findEntities<CameraFixture, Transform>()->each([this, direction, deltaTime](Entity& entity) {
    auto cameraFixture = entity.get<CameraFixture>();
    auto transform = entity.get<Transform>();

    transform->position += glm::vec3(direction, 0.0f) * _inputManager.getSensitivity() * cameraFixture->speed * deltaTime;
  });
}

}  // namespace linguine
