#pragma once

#include "System.h"

#include <glm/vec2.hpp>
#include <optional>
#include <unordered_map>

#include "InputManager.h"
#include "renderer/Renderer.h"
#include "TimeManager.h"

namespace linguine {

class GestureRecognitionSystem : public System {
  public:
    GestureRecognitionSystem(EntityManager& entityManager,
                             InputManager& inputManager,
                             Renderer& renderer,
                             TimeManager& timeManager)
        : System(entityManager), _inputManager(inputManager),
          _renderer(renderer), _timeManager(timeManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    constexpr static float _longPressThreshold = 0.5f;
    constexpr static float _tapDistanceThreshold = 0.05f;

    InputManager& _inputManager;
    Renderer& _renderer;
    TimeManager& _timeManager;

    struct GestureState {
      time_t startTime;
      std::optional<uint64_t> startEntityId;
      glm::vec2 startLocation;
      glm::vec2 lastLocation;
    };

    std::unordered_map<uint64_t, GestureState> _gestureStates;

    void onDownEvent(uint64_t id, const InputManager::Touch& touch);

    void onHoldEvent(uint64_t id, const InputManager::Touch& touch);

    void onUpEvent(uint64_t id, const InputManager::Touch& touch);
};

}  // namespace linguine
