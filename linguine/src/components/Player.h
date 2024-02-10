#pragma once

namespace linguine {

struct Player {
  enum State {
    Left,
    LeftToCenter,
    Center,
    CenterToRight,
    Right,
    RightToCenter,
    CenterToLeft
  };

  float acceleration = 0.0f;
  float speed = 5.0f;
  float maxSpeed = 10.0f;
  State state = Center;
  float lerp = 0.0f;
};

}  // namespace linguine
