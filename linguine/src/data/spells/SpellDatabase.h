#pragma once

#include <unordered_map>

#include "Spell.h"
#include "actions/Apply.h"
#include "actions/MassHeal.h"
#include "actions/ModifyHealth.h"
#include "effects/ModifyHealthOverTime.h"

namespace linguine {

class SpellDatabase {
  public:
    SpellDatabase(ServiceLocator& serviceLocator,
                  EntityManager& entityManager)
        : _spells {
              { 1, std::make_shared<Spell>("Base", 0.0f, 1.5f, glm::vec3(0.12477f, 0.56471f, 0.07421f), "",std::make_unique<ModifyHealth>(entityManager, 250)) },
              { 2, std::make_shared<Spell>("AoE", 0.0f, 6.0f, glm::vec3(0.12477f, 0.56471f, 0.07421f), "",std::make_unique<MassHeal>(entityManager, 200)) }
          } {}

    Spell& getSpellById(uint64_t id) {
      return *_spells[id];
    }

  private:
    std::unordered_map<uint64_t, std::shared_ptr<Spell>> _spells;
};

}  // namespace linguine
