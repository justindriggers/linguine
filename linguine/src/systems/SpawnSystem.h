#pragma once

#include "System.h"

#include <random>

#include "renderer/Renderer.h"

namespace linguine {

class SpawnSystem : public System {
  public:
    SpawnSystem(EntityManager& entityManager, Renderer& renderer);

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    Renderer& _renderer;

    void spawnPowerUp(glm::vec3 spawnPointPosition);

    void spawnAsteroid(glm::vec3 spawnPointPosition);

    void spawnObstacles(glm::vec3 spawnPointPosition);

    void spawnStars(glm::vec3 spawnPointPosition);
};

}  // namespace linguine
