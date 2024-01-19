#pragma once

#include "Scene.h"

#include "ServiceLocator.h"

namespace linguine {

class OptionsScene : public Scene {
  public:
    explicit OptionsScene(ServiceLocator& serviceLocator);
};

}  // namespace linguine
