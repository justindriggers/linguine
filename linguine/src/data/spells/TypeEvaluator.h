#pragma once

#include "Type.h"

namespace linguine {

class TypeEvaluator {
  public:
    static float calculateModifier(Type spellType, Type targetType) {
      switch (targetType) {
      case Fire:
        switch (spellType) {
        case Fire:
        case Storm:
          return Heal;
        case Water:
        case Earth:
          return Weak;
        default:
          return Base;
        }
      case Water:
        switch (spellType) {
        case Fire:
          return Resist;
        case Water:
          return Heal;
        case Storm:
          return Weak;
        default:
          return Base;
        }
      case Storm:
        switch (spellType) {
        case Storm:
          return Heal;
        default:
          return Base;
        }
      case Earth:
        switch (spellType) {
        case Fire:
        case Storm:
          return Resist;
        case Water:
        case Growth:
          return Weak;
        case Earth:
          return Heal;
        default:
          return Base;
        }
        break;
      case Decay:
        switch (spellType) {
        case Fire:
        case Growth:
          return Weak;
        case Decay:
          return Heal;
        default:
          return Base;
        }
        break;
      case Growth:
        switch (spellType) {
        case Fire:
        case Decay:
          return Weak;
        case Water:
        case Growth:
          return Heal;
        case Earth:
          return Resist;
        default:
          return Base;
        }
        break;
      default:
        throw std::runtime_error("Unsupported type application");
      }
    }

  private:
    constexpr static float Base = -1.0f;
    constexpr static float Resist = -0.5f;
    constexpr static float Weak = -2.0f;
    constexpr static float Heal = 1.0f;
};

}  // namespace linguine
