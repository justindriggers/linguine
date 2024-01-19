#pragma once

#include <memory>

namespace linguine {

class Scene;

class SceneManager {
  public:
    virtual void load(std::unique_ptr<Scene> scene) = 0;
};

}  // namespace linguine
