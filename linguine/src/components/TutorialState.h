#pragma once

namespace linguine {

struct TutorialState {
  enum class State {
    WaitingForScore,
    WaitingForHeal,
    Finished
  };

  State currentState = State::WaitingForScore;
  float elapsed = 3.0f;
  bool hasScored = false;
  bool hasHealed = false;
  bool hasFinished = false;
};

}  // namespace linguine
