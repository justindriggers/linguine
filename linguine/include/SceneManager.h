#pragma once

#include <memory>

#include "Scene.h"

namespace linguine {

class SceneManager {
  public:
    virtual void load(std::unique_ptr<Scene> scene) = 0;
};

}  // namespace linguine
