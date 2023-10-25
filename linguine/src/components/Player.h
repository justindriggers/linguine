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

  float acceleration = 0.1f;
  float speed = 1.0f;
  float maxSpeed = 50.0f;
  State state = Center;
  float lerp = 0.0f;
};

}  // namespace linguine
