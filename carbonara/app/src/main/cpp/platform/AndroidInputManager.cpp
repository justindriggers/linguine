#include "AndroidInputManager.h"

#include <vector>

#include "jni/Callbacks.h"

namespace linguine::carbonara {

AndroidInputManager::AndroidInputManager(android_app& app, const Viewport& viewport)
    : _app(app), _viewport(viewport) {
  android_app_set_motion_event_filter(&app, [](const GameActivityMotionEvent* motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return sourceClass == AINPUT_SOURCE_CLASS_POINTER;
  });

  registerSwipeListener(*this);
}

AndroidInputManager::~AndroidInputManager() {
  unregisterSwipeListener(*this);
}

void AndroidInputManager::pollEvents() {
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

  pollNativeEvents(_app);

  _currentLeftSwipe = _pendingLeftSwipe;
  _pendingLeftSwipe = false;

  _currentRightSwipe = _pendingRightSwipe;
  _pendingRightSwipe = false;

  auto* inputBuffer = android_app_swap_input_buffers(&_app);

  if (!inputBuffer) {
    return;
  }

  for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
    auto& motionEvent = inputBuffer->motionEvents[i];
    auto action = motionEvent.action;

    switch (action & AMOTION_EVENT_ACTION_MASK) {
    case AMOTION_EVENT_ACTION_DOWN:
    case AMOTION_EVENT_ACTION_POINTER_DOWN: {
      auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                          >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

      auto& pointer = motionEvent.pointers[pointerIndex];
      auto x = GameActivityPointerAxes_getX(&pointer);
      auto y = GameActivityPointerAxes_getY(&pointer);

      _active.insert({pointer.id, { x / static_cast<float>(_viewport.getWidth()),
                                   1.0f - y / static_cast<float>(_viewport.getHeight()),
                                   Down }});
      break;
    }

    case AMOTION_EVENT_ACTION_CANCEL:
    case AMOTION_EVENT_ACTION_UP:
    case AMOTION_EVENT_ACTION_POINTER_UP: {
      auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                          >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

      auto& pointer = motionEvent.pointers[pointerIndex];
      auto x = GameActivityPointerAxes_getX(&pointer);
      auto y = GameActivityPointerAxes_getY(&pointer);

      _active[pointer.id] = { x / static_cast<float>(_viewport.getWidth()),
                             1.0f - y / static_cast<float>(_viewport.getHeight()),
                             Up };
      break;
    }

    // Contains a snapshot of all pointer states
    case AMOTION_EVENT_ACTION_MOVE:
      for (auto index = 0; index < motionEvent.pointerCount; index++) {
        auto& pointer = motionEvent.pointers[index];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);
        _active[pointer.id] = { x / static_cast<float>(_viewport.getWidth()),
                               1.0f - y / static_cast<float>(_viewport.getHeight()),
                               Hold };
      }
      break;

    default:
      break;
    }
  }

  android_app_clear_motion_events(inputBuffer);
}

void AndroidInputManager::onLeftSwipe() {
  _pendingLeftSwipe = true;
}

void AndroidInputManager::onRightSwipe() {
  _pendingRightSwipe = true;
}

void AndroidInputManager::pollNativeEvents(android_app& app) {
  int events;
  android_poll_source* source;

  if (ALooper_pollAll(0, nullptr, &events, (void **) &source) >= 0) {
    if (source) {
      source->process(&app, source);
    }
  }
}

}  // namespace linguine::carbonara
