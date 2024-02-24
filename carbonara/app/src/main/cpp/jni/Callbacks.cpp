#include "Callbacks.h"

#include <set>

#include <jni.h>

namespace linguine::carbonara {

std::set<SwipeListener*> _swipeListeners{};

void registerSwipeListener(SwipeListener& swipeListener) {
  _swipeListeners.insert(&swipeListener);
}

void unregisterSwipeListener(SwipeListener& swipeListener) {
  _swipeListeners.erase(&swipeListener);
}

}  // namespace linguine::carbonara

extern "C" {

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_Native_onLeftSwipe() {
  for (auto* swipeListener : linguine::carbonara::_swipeListeners) {
    swipeListener->onLeftSwipe();
  }
}

JNIEXPORT void JNICALL Java_com_justindriggers_carbonara_Native_onRightSwipe() {
  for (auto* swipeListener : linguine::carbonara::_swipeListeners) {
    swipeListener->onRightSwipe();
  }
}

}
