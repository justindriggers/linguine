#pragma once

namespace linguine {

struct SpawnPoint {
  float distance = 15.0f;
  float lastSpawnPoint = 0.0f;
  float spawnChance = 1.0f;
  float powerUpInterval = 15.0f;
  float powerUpElapsed = 0.0f;
  std::unordered_map<uint64_t, float> powerUpCooldowns{};
};

}  // namespace linguine
