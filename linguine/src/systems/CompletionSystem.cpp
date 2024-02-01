#include "CompletionSystem.h"

#include "components/BoxCollider.h"
#include "components/Event.h"
#include "components/FinishLine.h"
#include "components/Hit.h"
#include "components/Player.h"
#include "components/Score.h"
#include "scenes/LevelCompletedScene.h"

namespace linguine {

void CompletionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<FinishLine, Hit>()->each([this](Entity& entity) {
    auto hit = entity.get<Hit>();

    for (auto entityId : hit->entityIds) {
      auto hitEntity = getEntityById(entityId);

      if (hitEntity->has<Player>()) {
        entity.remove<BoxCollider>();

        auto event = entity.add<Event>();
        event->secondsRemaining = 3.0f;
        event->function = [this]() {
          findEntities<Score>()->each([this](const Entity& scoreEntity) {
            auto score = scoreEntity.get<Score>();
            auto rate = static_cast<float>(score->points) / static_cast<float>(score->possiblePoints);

            auto stars = 0;

            if (rate >= 1.0f) {
              stars = 3;
            } else if (rate >= 0.75f) {
              stars = 2;
            } else if (rate > 0.0f) {
              stars = 1;
            }

            auto& sceneManager = _serviceLocator.get<SceneManager>();
            sceneManager.load(std::make_unique<LevelCompletedScene>(_serviceLocator, score->level, stars));
          });
        };

        return;
      }
    }
  });
}

}  // namespace linguine
