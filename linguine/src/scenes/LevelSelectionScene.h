#pragma once

#include "Scene.h"
#include "data/LevelDatabase.h"

namespace linguine {

class LevelSelectionScene : public Scene {
  public:
    explicit LevelSelectionScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator) {}

    void init() override;

  private:
    LevelDatabase _levelDatabase;
};

}  // namespace linguine
