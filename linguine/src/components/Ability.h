#pragma once

#include "data/spells/Spell.h"

namespace linguine {

struct Ability {
  explicit Ability(Spell& spell) : spell(spell) {}

  Spell& spell;
  float remainingCooldown = 0.0f;
};

}