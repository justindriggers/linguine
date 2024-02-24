#pragma once

namespace linguine::carbonara {

class SwipeListener {
  public:
    virtual void onLeftSwipe() = 0;

    virtual void onRightSwipe() = 0;
};

}  // namespace linguine::carbonara
