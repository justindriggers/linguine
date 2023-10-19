#pragma once

#include <unordered_map>

#include "Spell.h"
#include "actions/Apply.h"
#include "actions/ModifyHealth.h"
#include "effects/ModifyHealthOverTime.h"

namespace linguine {

class SpellDatabase {
  public:
    SpellDatabase(ServiceLocator& serviceLocator,
                  EntityManager& entityManager)
        : _spells {
              { 0, std::make_shared<Spell>("Spark", 0.0f, 6.0f, Type::Storm, glm::vec3(1.0f, 0.7991f, 0.12214f), "Cooldown   6s\nPower      175", std::make_unique<ModifyHealth>(entityManager, Type::Storm, 50)) },
              { 1, std::make_shared<Spell>("Growth", 0.0f, 3.5f, Type::Growth, glm::vec3(0.12477f, 0.56471f, 0.07421f), "Cooldown 2.5s\nPower      250",std::make_unique<ModifyHealth>(entityManager, Type::Growth, 75)) },
              { 2, std::make_shared<Spell>("Splash", 0.0f, 4.0f, Type::Water, glm::vec3(0.02519f, 0.80695f, 0.90466f), "Cooldown 1.5s\nPower      50/s for 12s",std::make_unique<Apply>(serviceLocator, entityManager, std::make_unique<ModifyHealthOverTime>(entityManager, Type::Water, glm::vec3(0.02519f, 0.80695f, 0.90466f), 12.0f, 6, 25))) },
              { 3, std::make_shared<Spell>("Scorch", 0.0f, 4.0f, Type::Fire, glm::vec3(0.78354f, 0.04374f, 0.05781f), "Cooldown 1.5s\nPower      35/s for 18s",std::make_unique<Apply>(serviceLocator, entityManager, std::make_unique<ModifyHealthOverTime>(entityManager, Type::Fire, glm::vec3(0.78354f, 0.04374f, 0.05781f), 18.0f, 6, 35))) }
          } {}

    Spell& getSpellById(uint64_t id) {
      return *_spells[id];
    }

  private:
    std::unordered_map<uint64_t, std::shared_ptr<Spell>> _spells;
};

}  // namespace linguine
