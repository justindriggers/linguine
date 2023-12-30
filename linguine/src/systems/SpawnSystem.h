#pragma once

#include "System.h"

#include <random>

#include "renderer/Renderer.h"

namespace linguine {

class SpawnSystem : public System {
  public:
    SpawnSystem(EntityManager& entityManager, Renderer& renderer)
        : System(entityManager), _renderer(renderer) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    Renderer& _renderer;

    void spawnPowerUp(float y);

    void spawnAsteroid(float y);

    void spawnObstacles(float y);

    void spawnStars(float y);

    void spawnMovementText(float y);

    void spawnScoringText(float y);

    void spawnHealingText(float y);

    void spawnEvasionText(float y);
};

}  // namespace linguine
