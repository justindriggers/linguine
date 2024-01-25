#pragma once

#include "Scene.h"

namespace linguine {

class NewPlayerScene : public Scene {
  public:
    explicit NewPlayerScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator) {}

    void init() override;
};

}  // namespace linguine
