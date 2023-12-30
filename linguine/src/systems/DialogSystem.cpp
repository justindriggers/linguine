#include "DialogSystem.h"
#include "components/Dialog.h"
#include "components/Drawable.h"
#include "components/Selectable.h"
#include "components/Text.h"

namespace linguine {

void DialogSystem::update(float deltaTime) {
  findEntities<Dialog, Drawable>()->each([](const Entity& entity) {
    auto dialog = entity.get<Dialog>();
    auto drawable = entity.get<Drawable>();
    drawable->renderable->setEnabled(dialog->enabled);
  });

  findEntities<Dialog, Text>()->each([](const Entity& entity) {
    auto dialog = entity.get<Dialog>();
    auto text = entity.get<Text>();
    text->renderable->setEnabled(dialog->enabled);
  });

  findEntities<Dialog, Selectable>()->each([](const Entity& entity) {
    auto dialog = entity.get<Dialog>();
    auto selectable = entity.get<Selectable>();
    selectable->renderable->setEnabled(dialog->enabled);
  });
}

}  // namespace linguine
