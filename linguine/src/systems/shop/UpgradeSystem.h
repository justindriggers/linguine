#pragma once

#include "System.h"

#include "SceneManager.h"
#include "ServiceLocator.h"
#include "data/upgrades/UpgradeDatabase.h"
#include "renderer/Renderer.h"

namespace linguine {

class UpgradeSystem : public System {
  public:
    UpgradeSystem(EntityManager& entityManager,
                  Renderer& renderer,
                  SceneManager& sceneManager,
                  ServiceLocator& serviceLocator,
                  UpgradeDatabase& upgradeDatabase);

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    SceneManager& _sceneManager;
    ServiceLocator& _serviceLocator;
    UpgradeDatabase& _upgradeDatabase;
};

}  // namespace linguine
