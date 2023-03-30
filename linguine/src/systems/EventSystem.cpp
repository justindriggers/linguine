#include "EventSystem.h"

#include "components/Event.h"

namespace linguine {

void EventSystem::update(float deltaTime) {
  findEntities<Event>()->each([deltaTime](Entity& entity) {
    auto event = entity.get<Event>();

    if (!event->useFixedTimeStep) {
      event->secondsRemaining -= deltaTime;

      if (event->secondsRemaining <= 0.0f) {
        event->function();
        entity.remove<Event>();
      }
    }
  });
}

void EventSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Event>()->each([fixedDeltaTime](Entity& entity) {
    auto event = entity.get<Event>();

    if (event->useFixedTimeStep) {
      event->secondsRemaining -= fixedDeltaTime;

      if (event->secondsRemaining <= 0.0f) {
        event->function();
        entity.remove<Event>();
      }
    }
  });
}

}  // namespace linguine
