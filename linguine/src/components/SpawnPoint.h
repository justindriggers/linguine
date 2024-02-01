#pragma once

namespace linguine {

struct SpawnPoint {
  float distance = 15.0f;
  float lastSpawnPoint = 0.0f;
  float spawnChance = 1.0f;
  float obstacleSpawnChance = 0.65f;
  float wallSpawnChance = 0.0f;
  float powerUpInterval = 15.0f;
  float powerUpElapsed = 0.0f;
  bool speedBoostEnabled = true;
  bool regenEnabled = true;
  bool reviveEnabled = true;
  bool timeWarpEnabled = true;
  std::unordered_map<uint64_t, float> powerUpCooldowns{};
  float requiredDistance = 0.0f;
};

}  // namespace linguine
