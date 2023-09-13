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
              { 0, std::make_shared<Spell>("Infuse", 0.0f, 6.0f, glm::vec3(1.0f, 0.7991f, 0.12214f), "Cast time  Instant\nCooldown   6s\nPower      175", std::make_unique<Heal>(entityManager, 175)) },
              { 1, std::make_shared<Spell>("Restore", 2.5f, 0.0f, glm::vec3(0.12477f, 0.56471f, 0.07421f), "Cast time  2.5s\nCooldown   None\nPower      250",std::make_unique<Heal>(entityManager, 250)) },
              { 2, std::make_shared<Spell>("Siphon", 1.5f, 0.0f, glm::vec3(0.78354f, 0.04374f, 0.05781f), "Cast time  1.5s\nCooldown   None\nPower      50/s over 12s",std::make_unique<Apply>(serviceLocator, entityManager, std::make_unique<HealOverTime>(entityManager, 12.0f, 6, 50))) }
          } {}

    Spell& getSpellById(uint64_t id) {
      return *_spells[id];
    }

  private:
    std::unordered_map<uint64_t, std::shared_ptr<Spell>> _spells;
};

}  // namespace linguine
