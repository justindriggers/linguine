#pragma once

namespace linguine {

struct TutorialState {
  enum class State {
    Movement,
    WaitingForMovement,
    Scoring,
    WaitingForScore,
    Healing,
    WaitingForHeal,
    Evasion,
    Finished
  };

  State currentState = State::Movement;
  float elapsed = 0.0f;
  bool hasMoved = false;
  int asteroidsSpawned = 0;
  bool hasScored = false;
  bool hasHealed = false;
};

}  // namespace linguine
