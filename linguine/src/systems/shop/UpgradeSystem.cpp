#include "UpgradeSystem.h"

#include "components/CostLabel.h"
#include "components/Disabled.h"
#include "components/Drawable.h"
#include "components/Enabled.h"
#include "components/LongPressed.h"
#include "components/Pressed.h"
#include "components/RankIndicator.h"
#include "components/Selectable.h"
#include "components/Tapped.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "components/shop/PlayButton.h"
#include "components/shop/PurchaseButton.h"
#include "scenes/InfiniteRunnerScene.h"

namespace linguine {

UpgradeSystem::UpgradeSystem(EntityManager& entityManager,
                             Renderer& renderer,
                             SaveManager& saveManager,
                             SceneManager& sceneManager,
                             ServiceLocator& serviceLocator,
                             UpgradeDatabase& upgradeDatabase)
    : System(entityManager), _saveManager(saveManager),
      _sceneManager(sceneManager), _serviceLocator(serviceLocator),
      _upgradeDatabase(upgradeDatabase) {
  auto y = 151.0f;

  for (const auto& upgradeEntry : _upgradeDatabase.getUpgrades()) {
    auto& upgrade = upgradeEntry.second;

    auto headerEntity = createEntity();

    auto headerTransform = headerEntity->add<Transform>();
    headerTransform->position = { -106.0f, y, 0.0f };
    headerTransform->scale = { 8.0f, 8.0f, 0.0f };

    auto headerText = headerEntity->add<Text>();
    headerText->feature = new TextFeature();
    headerText->feature->text = upgrade.name;
    headerText->renderable = renderer.create(std::unique_ptr<TextFeature>(headerText->feature), UI);
    headerText.setRemovalListener([headerText](const Entity e) {
      headerText->renderable->destroy();
    });

    {
      auto headerBackgroundEntity = createEntity();

      auto headerBackgroundTransform = headerBackgroundEntity->add<Transform>();
      headerBackgroundTransform->position = { 0.0f, y, 9.0f };
      headerBackgroundTransform->scale = { 228.0f, 16.0f, 1.0f };

      auto headerBackgroundDrawable = headerBackgroundEntity->add<Drawable>();
      headerBackgroundDrawable->feature = new ColoredFeature();
      headerBackgroundDrawable->feature->meshType = Quad;
      headerBackgroundDrawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
      headerBackgroundDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(headerBackgroundDrawable->feature), UI);
      headerBackgroundDrawable.setRemovalListener([headerBackgroundDrawable](const Entity e) {
        headerBackgroundDrawable->renderable->destroy();
      });
    }

    y -= 36.0f;

    {
      auto backgroundEntity = createEntity();

      auto backgroundTransform = backgroundEntity->add<Transform>();
      backgroundTransform->position = { 0.0f, y, 9.0f };
      backgroundTransform->scale = { 228.0f, 56.0f, 1.0f };

      auto backgroundDrawable = backgroundEntity->add<Drawable>();
      backgroundDrawable->feature = new ColoredFeature();
      backgroundDrawable->feature->meshType = Quad;
      backgroundDrawable->feature->color = { 1.0f, 1.0f, 1.0f };
      backgroundDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(backgroundDrawable->feature), UI);
      backgroundDrawable.setRemovalListener([backgroundDrawable](const Entity e) {
        backgroundDrawable->renderable->destroy();
      });
    }

    y += 22.0f;

    {
      auto descriptionEntity = createEntity();

      auto descriptionTransform = descriptionEntity->add<Transform>();
      descriptionTransform->position = { -107.0f, y, 0.0f };
      descriptionTransform->scale = { 6.0f, 6.0f, 0.0f };

      auto descriptionText = descriptionEntity->add<Text>();
      descriptionText->feature = new TextFeature();
      descriptionText->feature->text = upgrade.description;
      descriptionText->feature->color = { 0.0f, 0.0f, 0.0f };
      descriptionText->renderable = renderer.create(std::unique_ptr<TextFeature>(descriptionText->feature), UI);
      descriptionText.setRemovalListener([descriptionText](const Entity e) {
        descriptionText->renderable->destroy();
      });
    }

    y -= 14.0f;

    {
      auto rankBackgroundEntity = createEntity();

      auto rankBackgroundTransform = rankBackgroundEntity->add<Transform>();
      rankBackgroundTransform->position = { 0.0f, y, 5.0f };
      rankBackgroundTransform->scale = { 220.0f, 16.0f, 1.0f };

      auto rankBackgroundDrawable = rankBackgroundEntity->add<Drawable>();
      rankBackgroundDrawable->feature = new ColoredFeature();
      rankBackgroundDrawable->feature->meshType = Quad;
      rankBackgroundDrawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
      rankBackgroundDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(rankBackgroundDrawable->feature), UI);
      rankBackgroundDrawable.setRemovalListener([rankBackgroundDrawable](const Entity e) {
        rankBackgroundDrawable->renderable->destroy();
      });
    }

    for (auto i = 0; i < upgrade.rankCosts.size(); ++i) {
      auto rankEntity = createEntity();
      rankEntity->add<RankIndicator>(upgradeEntry.first, i);

      auto width = 200.0f / static_cast<float>(upgrade.rankCosts.size());
      auto spacing = 12.0f / static_cast<float>(upgrade.rankCosts.size() - 1);

      auto rankTransform = rankEntity->add<Transform>();
      rankTransform->position = { -106.0f + width / 2.0f + static_cast<float>(i) * (width + spacing), y, 0.0f };
      rankTransform->scale = { width, 8.0f, 0.0f };

      auto rankDrawable = rankEntity->add<Drawable>();
      rankDrawable->feature = new ColoredFeature();
      rankDrawable->feature->meshType = Quad;

      rankDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(rankDrawable->feature), UI);
      rankDrawable.setRemovalListener([rankDrawable](const Entity e) {
        rankDrawable->renderable->destroy();
      });
    }

    y -= 22.0f;

    {
      auto upgradeButton = createEntity();
      upgradeButton->add<PurchaseButton>()->upgradeId = upgradeEntry.first;

      auto upgradeTransform = upgradeButton->add<Transform>();
      upgradeTransform->position = { 0.0f, y, 0.0f };
      upgradeTransform->scale = { 220.0f, 20.0f, 0.0f };

      auto upgradeDrawable = upgradeButton->add<Drawable>();
      upgradeDrawable->feature = new ColoredFeature();
      upgradeDrawable->feature->meshType = Quad;
      upgradeDrawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
      upgradeDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(upgradeDrawable->feature), UI);
      upgradeDrawable.setRemovalListener([upgradeDrawable](const Entity e) {
        upgradeDrawable->renderable->destroy();
      });

      auto upgradeSelectable = upgradeButton->add<Selectable>();
      upgradeSelectable->feature = new SelectableFeature();
      upgradeSelectable->feature->entityId = upgradeButton->getId();
      upgradeSelectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(upgradeSelectable->feature), UI);
      upgradeSelectable.setRemovalListener([upgradeSelectable](const Entity e) {
        upgradeSelectable->renderable->destroy();
      });

      {
        auto costEntity = createEntity();
        costEntity->add<CostLabel>(upgradeEntry.first);

        auto costTransform = costEntity->add<Transform>();
        costTransform->position = { 0.0f, y, 0.0f };
        costTransform->scale = { 10.0f, 10.0f, 0.0f };

        auto costText = costEntity->add<Text>();
        costText->feature = new TextFeature();
        costText->renderable = renderer.create(std::unique_ptr<TextFeature>(costText->feature), UI);
        costText.setRemovalListener([costText](const Entity e) {
          costText->renderable->destroy();
        });
      }
    }

    y -= 32.0f;
  }
}

void UpgradeSystem::update(float deltaTime) {
  findEntities<RankIndicator, Drawable>()->each([this](const Entity& entity) {
    auto rankIndicator = entity.get<RankIndicator>();
    auto drawable = entity.get<Drawable>();

    if (rankIndicator->rank <= _saveManager.getRank(rankIndicator->upgradeId)) {
      drawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
    } else {
      drawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
    }
  });

  findEntities<CostLabel, Text, Transform>()->each([this](const Entity& entity) {
    auto costLabel = entity.get<CostLabel>();
    auto text = entity.get<Text>();
    auto transform = entity.get<Transform>();

    auto& upgrade = _upgradeDatabase.getById(costLabel->upgradeId);
    auto nextRank = _saveManager.getRank(costLabel->upgradeId) + 1;

    if (nextRank < upgrade.rankCosts.size()) {
      text->feature->text = std::to_string(upgrade.rankCosts[nextRank - 1]);
      transform->position.x = -static_cast<float>(text->feature->text.size()) / 2.0f * 10.0f + 5.0f;
    } else {
      text->renderable->setEnabled(false);
    }
  });

  findEntities<PurchaseButton>()->each([this](Entity& entity) {
    auto purchaseButton = entity.get<PurchaseButton>();

    auto& upgrade = _upgradeDatabase.getById(purchaseButton->upgradeId);
    auto nextRank = _saveManager.getRank(purchaseButton->upgradeId) + 1;

    if (nextRank < upgrade.rankCosts.size()
        && upgrade.rankCosts[nextRank - 1] <= _saveManager.getPoints()) {
      if (entity.has<Disabled>()) {
        entity.remove<Disabled>();
      }

      if (!entity.has<Enabled>()) {
        entity.add<Enabled>();
      }
    } else {
      if (!entity.has<Disabled>()) {
        entity.add<Disabled>();
      }

      if (entity.has<Enabled>()) {
        entity.remove<Enabled>();
      }
    }
  });

  findEntities<PurchaseButton, Enabled>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  });

  findEntities<PurchaseButton, Drawable, Enabled, LongPressed>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.97345f, 0.36625f, 0.00561f };
  });

  findEntities<PurchaseButton, Drawable, Enabled, Pressed>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
  });

  findEntities<PurchaseButton, Drawable, Disabled>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
  });

  findEntities<PurchaseButton, Enabled, Tapped>()->each([this](const Entity& entity) {
    auto purchaseButton = entity.get<PurchaseButton>();

    auto& upgrade = _upgradeDatabase.getById(purchaseButton->upgradeId);
    auto nextRank = _saveManager.getRank(purchaseButton->upgradeId) + 1;

    _saveManager.removePoints(upgrade.rankCosts[nextRank - 1]);
    _saveManager.increaseRank(purchaseButton->upgradeId);
  });

  findEntities<PlayButton, Drawable, Enabled>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
  });

  findEntities<PlayButton, Drawable, Enabled, LongPressed>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.007f, 0.01521f, 0.04667f };
  });

  findEntities<PlayButton, Drawable, Enabled, Pressed>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
  });

  findEntities<PlayButton, Drawable, Disabled>()->each([](const Entity& entity) {
    auto drawable = entity.get<Drawable>();
    drawable->feature->color = { 0.78354f, 0.78354f, 0.78354f };
  });

  findEntities<PlayButton, Tapped>()->each([this](const Entity& entity) {
    _sceneManager.load(std::make_unique<InfiniteRunnerScene>(_serviceLocator));
  });
}

}  // namespace linguine
