#pragma once

#include <glm/vec2.hpp>

#include "RoomLayout.h"
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

    bool layout(EntityManager& entityManager,
                ServiceLocator& serviceLocator,
                Grid& grid,
                RoomLayout& roomLayout,
                glm::ivec2 origin) const {
      for (auto x = -1; x < getWidth() + 1; ++x) {
        for (auto y = -1; y < getHeight() + 1; ++y) {
          auto gridPosition = glm::ivec2(origin.x + x, origin.y + y);

          if (roomLayout.isOccupied(gridPosition)) {
            return false;
          }
        }
      }

      auto& renderer = serviceLocator.get<Renderer>();

      for (auto x = -1; x < getWidth() + 1; ++x) {
        for (auto y = -1; y < getHeight() + 1; ++y) {
          auto location = glm::ivec2(x, y);

          if ((x == -1 || x == getWidth() || y == -1 || y == getHeight())
              && (location != getNorthDoor() && location != getSouthDoor() && location != getEastDoor() && location != getWestDoor())) {
            continue;
          }

          auto entity = entityManager.create();
          entity->add<Tile>();

          auto gridPosition = glm::ivec2(origin.x + x, origin.y + y);

          auto transform = entity->add<Transform>();
          transform->position = glm::vec3(grid.getWorldPosition(gridPosition), 5.0f);
          transform->scale = glm::vec3(0.9f);

          auto drawable = entity->add<Drawable>();
          drawable->feature = new ColoredFeature();
          drawable->feature->meshType = Quad;
          drawable->feature->color = glm::vec3(0.06f, 0.06f, 0.06f);
          drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
          drawable.setRemovalListener([drawable](const Entity e) {
            drawable->renderable->destroy();
          });

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

      roomLayout.add(origin, {getWidth(), getHeight()});
      roomLayout.add(origin + getNorthDoor(), {1, 1});
      roomLayout.add(origin + getSouthDoor(), {1, 1});
      roomLayout.add(origin + getEastDoor(), {1, 1});
      roomLayout.add(origin + getWestDoor(), {1, 1});
      return true;
    }

    void enclose(EntityManager& entityManager,
                 ServiceLocator& serviceLocator,
                 Grid& grid,
                 RoomLayout& roomLayout,
                 glm::ivec2 origin) const {
      auto& renderer = serviceLocator.get<Renderer>();

      for (auto x = -1; x < getWidth() + 1; ++x) {
        for (auto y = -1; y < getHeight() + 1; ++y) {
          if (x == -1 || x == getWidth() || y == -1 || y == getHeight()) {
            auto location = glm::ivec2(x, y);

            auto shouldClose = (location != getNorthDoor() && location != getSouthDoor() && location != getEastDoor() && location != getWestDoor())
                               || (location == getNorthDoor() && !roomLayout.isOccupied(origin + location + glm::ivec2(0, 1)))
                               || (location == getSouthDoor() && !roomLayout.isOccupied(origin + location - glm::ivec2(0, 1)))
                               || (location == getEastDoor() && !roomLayout.isOccupied(origin + location + glm::ivec2(1, 0)))
                               || (location == getWestDoor() && !roomLayout.isOccupied(origin + location - glm::ivec2(1, 0)));

            if (shouldClose) {
              grid.addObstruction({origin.x + x, origin.y + y}, {1, 1});
              roomLayout.remove({origin.x + x, origin.y + y}, {1, 1});

              auto entity = entityManager.create();
              entity->add<Tile>();

              auto gridPosition = glm::ivec2(origin.x + x, origin.y + y);

              auto transform = entity->add<Transform>();
              transform->position = glm::vec3(grid.getWorldPosition(gridPosition), 4.0f);
              transform->scale = glm::vec3(0.95f);

              auto drawable = entity->add<Drawable>();
              drawable->feature = new ColoredFeature();
              drawable->feature->meshType = Quad;
              drawable->feature->color = glm::vec3(0.26f, 0.26f, 0.26f);
              drawable->renderable = renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
              drawable.setRemovalListener([drawable](const Entity e) {
                drawable->renderable->destroy();
              });
            }
          }
        }
      }
    }

  private:
    int _width;
    int _height;

    glm::ivec2 _northDoor;
    glm::ivec2 _southDoor;
    glm::ivec2 _eastDoor;
    glm::ivec2 _westDoor;
};

}  // namespace linguine
