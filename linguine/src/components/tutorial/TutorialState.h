#pragma once

namespace linguine {

enum Stage {
  TeachTargeting,
  TeachCasting,
  Completed
};

struct TutorialState {
  Stage stage = TeachTargeting;
};

}  // namespace linguine
