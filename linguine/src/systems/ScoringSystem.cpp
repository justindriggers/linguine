#include "ScoringSystem.h"

#include "components/Alive.h"
#include "components/Asteroid.h"
#include "components/Bomb.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/Player.h"
#include "components/PowerUp.h"
#include "components/Score.h"
#include "components/Text.h"

namespace linguine {

void ScoringSystem::update(float deltaTime) {
  findEntities<Score, Text>()->each([](const Entity& entity) {
    auto score = entity.get<Score>();
    auto text = entity.get<Text>();

    text->feature->text = std::to_string(score->points);
  });
}

void ScoringSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Score>()->each([this](const Entity& scoreEntity) {
    auto score = scoreEntity.get<Score>();

    findEntities<Player, Hit>()->each([this, &score](Entity& playerEntity) {
      auto hit = playerEntity.get<Hit>();

      for (const auto& hitEntityId : hit->entityIds) {
        auto hitEntity = getEntityById(hitEntityId);

        if (hitEntity->has<Asteroid>()) {
          auto asteroid = hitEntity->get<Asteroid>();
          score->points += asteroid->points;

          auto living = findEntities<Health, Alive>()->get();
          auto distribution = std::uniform_int_distribution(0, static_cast<int>(living.size() - 1));

          auto randomIndex = distribution(_random);
          auto partyMember = living[randomIndex];

          auto health = partyMember->get<Health>();
          health->current = glm::clamp(health->current - 50 * asteroid->points, 0, health->max);

          hitEntity->destroy();
        } else if (hitEntity->has<PowerUp>()) {
          auto powerUp = hitEntity->get<PowerUp>();

          switch (powerUp->type) {
          case PowerUp::MassHeal:
            _spellDatabase.getSpellById(2).action->execute(playerEntity);
            break;
          }

          hitEntity->destroy();
        } else if (hitEntity->has<Bomb>()) {
          _spellDatabase.getSpellById(3).action->execute(playerEntity);
          hitEntity->destroy();
        }
      }
    });
  });
}

}  // namespace linguine
