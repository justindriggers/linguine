#pragma once

namespace linguine {

struct SpawnPoint {
  float interval = 1.0f;
  float elapsed = 0.0f;
  float spawnChance = 1.0f;
  float powerUpInterval = 15.0f;
  float powerUpElapsed = 0.0f;
  float starInterval = 0.25f;
  float starElapsed = 0.0f;
};

}  // namespace linguine
