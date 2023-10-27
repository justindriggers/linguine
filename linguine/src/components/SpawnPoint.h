#pragma once

namespace linguine {

struct SpawnPoint {
  float distance = 15.0f;
  float lastSpawnPoint = 0.0f;
  float spawnChance = 1.0f;
  float powerUpInterval = 15.0f;
  float powerUpElapsed = 0.0f;
  float starDistance = 1.25f;
  float lastStarSpawnPoint = 0.0f;
};

}  // namespace linguine
