#pragma once

#include <glm/vec2.hpp>

#include "ServiceLocator.h"
#include "components/Selectable.h"
#include "components/Tile.h"
#include "entity/Entity.h"
#include "entity/EntityManager.h"

namespace linguine {

class Room {
  public:
    Room(int width, int height, glm::ivec2 northDoor, glm::ivec2 southDoor,
         glm::ivec2 eastDoor, glm::ivec2 westDoor)
        : _width(width), _height(height), _northDoor(northDoor),
          _southDoor(southDoor), _eastDoor(eastDoor), _westDoor(westDoor) {}

    virtual ~Room() = default;

    [[nodiscard]] int getWidth() const {
      return _width;
    }

    [[nodiscard]] int getHeight() const {
      return _height;
    }

    [[nodiscard]] glm::ivec2 getNorthDoor() const {
      return _northDoor;
    }

    [[nodiscard]] glm::ivec2 getSouthDoor() const {
      return _southDoor;
    }

    [[nodiscard]] glm::ivec2 getEastDoor() const {
      return _eastDoor;
    }

    [[nodiscard]] glm::ivec2 getWestDoor() const {
      return _westDoor;
    }

    void layout(EntityManager& entityManager,
                ServiceLocator& serviceLocator,
                Grid& grid,
                glm::ivec2 origin) const {
      auto& renderer = serviceLocator.get<Renderer>();

      for (auto x = -1; x < getWidth() + 1; ++x) {
        for (auto y = -1; y < getHeight() + 1; ++y) {
          auto entity = entityManager.create();
          entity->add<Tile>();

          auto gridPosition = glm::ivec2(origin.x + x, origin.y + y);

          auto transform = entity->add<Transform>();
          transform->position = glm::vec3(grid.getWorldPosition(gridPosition), 5.0f);
          transform->scale = glm::vec3(0.9f);

          auto drawable = entity->add<Drawable>();
          drawable->feature = new ColoredFeature();
          drawable->feature->meshType = Quad;
          drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
          drawable.setRemovalListener([drawable](const Entity e) {
            drawable->renderable->destroy();
          });

          auto location = glm::ivec2(x, y);

          if ((x == -1 || x == getWidth() || y == -1 || y == getHeight())
              && location != getNorthDoor() && location != getSouthDoor()
              && location != getEastDoor() && location != getWestDoor()) {
            grid.addObstruction({origin.x + x, origin.y + y}, {1, 1});
            drawable->feature->color = glm::vec3(0.26f, 0.26f, 0.26f);
          } else {
            drawable->feature->color = glm::vec3(0.06f, 0.06f, 0.06f);

            auto selectable = entity->add<Selectable>();
            selectable->feature = new SelectableFeature();
            selectable->feature->meshType = Quad;
            selectable->feature->entityId = entity->getId();
            selectable->renderable = renderer.create(std::unique_ptr<SelectableFeature>(selectable->feature));
            selectable.setRemovalListener([selectable](const Entity e) {
              selectable->renderable->destroy();
            });
          }
        }
      }

      drawDoor(entityManager, renderer, grid, origin, _northDoor);
      drawDoor(entityManager, renderer, grid, origin, _southDoor);
      drawDoor(entityManager, renderer, grid, origin, _eastDoor);
      drawDoor(entityManager, renderer, grid, origin, _westDoor);
    }

  private:
    int _width;
    int _height;

    glm::ivec2 _northDoor;
    glm::ivec2 _southDoor;
    glm::ivec2 _eastDoor;
    glm::ivec2 _westDoor;

    static void drawDoor(EntityManager& entityManager, Renderer& renderer,
                         Grid& grid, glm::ivec2 origin,
                         glm::ivec2 doorPosition) {
      auto doorEntity = entityManager.create();

      auto gridPosition = glm::ivec2(origin.x + doorPosition.x, origin.y + doorPosition.y);

      auto doorTransform = doorEntity->add<Transform>();
      doorTransform->position = glm::vec3(grid.getWorldPosition(gridPosition), 4.0f);
      doorTransform->scale = glm::vec3(0.8f);

      auto doorDrawable = doorEntity->add<Drawable>();
      doorDrawable->feature = new ColoredFeature();
      doorDrawable->feature->meshType = Quad;
      doorDrawable->feature->color = glm::vec3(0.46f, 0.46f, 0.46f);
      doorDrawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(doorDrawable->feature));
      doorDrawable.setRemovalListener([doorDrawable](const Entity e) {
        doorDrawable->renderable->destroy();
      });
    }
};

}  // namespace linguine
