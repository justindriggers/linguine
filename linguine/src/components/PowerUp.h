#pragma once

namespace linguine {

struct PowerUp {
  enum class Type {
    MassHeal,
    Revive,
    SpeedBoost,
    TimeWarp
  };

  Type type = Type::SpeedBoost;
};

}  // namespace linguine
