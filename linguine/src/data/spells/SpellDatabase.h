#pragma once

#include <unordered_map>

#include "Spell.h"
#include "actions/Apply.h"
#include "actions/Heal.h"
#include "effects/HealOverTime.h"

namespace linguine {

class SpellDatabase {
  public:
    SpellDatabase(ServiceLocator& serviceLocator,
                  EntityManager& entityManager)
        : _spells {
              { 0, std::make_shared<Spell>(0.0f, 6.0f, glm::vec3(1.0f, 1.0f, 0.0f), "Cast time  Instant\nCooldown   6s\nPower      175", std::make_unique<Heal>(entityManager, 175)) },
              { 1, std::make_shared<Spell>(2.5f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), "Cast time  2.5s\nCooldown   None\nPower      250",std::make_unique<Heal>(entityManager, 250)) },
              { 2, std::make_shared<Spell>(1.5f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), "Cast time  1.5s\nCooldown   None\nPower      50/s over 12s",std::make_unique<Apply>(serviceLocator, entityManager, std::make_unique<HealOverTime>(entityManager, 12.0f, 6, 50))) }
          } {}

    Spell& getSpellById(uint64_t id) {
      return *_spells[id];
    }

  private:
    std::unordered_map<uint64_t, std::shared_ptr<Spell>> _spells;
};

}  // namespace linguine
