#pragma once

#include "Scene.h"

#include "data/LevelDatabase.h"

namespace linguine {

class NewPlayerScene : public Scene {
  public:
    explicit NewPlayerScene(ServiceLocator& serviceLocator)
        : Scene(serviceLocator) {}

    void init() override;

  private:
    LevelDatabase _levelDatabase;
};

}  // namespace linguine
