#pragma once

#include "data/spells/Spell.h"
#include "input/Key.h"

namespace linguine {

struct Ability {
  Ability(Spell& spell, Key key) : spell(spell), key(key) {}

  Spell& spell;
  Key key;
  float remainingCooldown = 0.0f;
};

}