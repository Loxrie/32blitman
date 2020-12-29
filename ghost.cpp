#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Rect ghostAnims[8] = {
  // Left
  Rect(0,4,2,2),
  Rect(2,4,2,2),
  // Right
  Rect(4,14,2,2),
  Rect(6,12,2,2),
  // Up
  Rect(8,14,2,2),
  Rect(10,14,2,2),
  // Down
  Rect(12,14,2,2),
  Rect(14,14,2,2)
};


Ghost::Ghost() {
  size = Size(16, 16);
  location = Point((19*8) + 4,15*8);
  target = Vec2(player.location);

  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  
  state = ghostState::CHASE;

  sprite = 0;

  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
        printf("Ghost::ghost collision detected %d:%d\n", tile_pt.x, tile_pt.y);
        this->moving_to = entityType::WALL;
    }
  };
}

uint32_t Ghost::invertDirection() {
  uint32_t i_direction = 0;
  switch(direction) {
    case Button::DPAD_LEFT:
      i_direction = Button::DPAD_RIGHT;
      break;
    case Button::DPAD_RIGHT:
      i_direction = Button::DPAD_LEFT;
      break;
    case Button::DPAD_UP:
      i_direction = Button::DPAD_DOWN;
      break;
    case Button::DPAD_DOWN:
      i_direction = Button::DPAD_UP;
      break;
  }
  return i_direction;
}

Rect Ghost::center(Point pos) {
  return Rect(pos.x + (size.w/4), pos.y + (size.h/4), size.h/2, size.w/2);
}

uint32_t Ghost::direction_to_target() {
  /**
   * https://gameinternals.com/understanding-pac-man-ghost-behavior
   *
   * When a decision about which direction to turn is necessary, 
   * the choice is made based on which tile adjoining the intersection 
   * will put the ghost nearest to its target tile, measured in a straight line.
   * The distance from every possibility to the target tile is measured, and whichever
   * tile is closest to the target will be selected. 
   */

  uint32_t where_will_we_go = 0;
  Point decision_tile = tile(location);
  Point target_tile = tile(target);
  
  uint32_t inverted_direction = invertDirection();
  uint32_t index = decision_tile.y * level_width + decision_tile.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    printf("Ghost::direction_to_target freezing ghost, no mapping.\n");
    return 0;
  }

  float min_distance = 600.f;
  uint32_t min_direction;
  for (uint32_t exit : search->second) {
    if (exit == inverted_direction) {
      continue;
    }

    Vec2 vector;
    switch (exit) {
      case Button::DPAD_LEFT:
        vector = Vec2(-2,0);
        break;
      case Button::DPAD_RIGHT:
        vector = Vec2(2,0);
        break;
      case Button::DPAD_UP:
        vector = Vec2(0,-2);
        break;
      case Button::DPAD_DOWN:
        vector = Vec2(0,2);
        break;
    }
    Point source_tile = Point(decision_tile.x + vector.x, decision_tile.y + vector.y);
    // printf("Ghost::direction_to_target %d:%d\n", source_tile.x, source_tile.y);
    float distance = fabs(Vec2(target_tile.x - source_tile.x, target_tile.y - source_tile.y).length());
    if (distance < min_distance) {
      min_distance = distance;
      min_direction = exit;
    }
  }

  return min_direction;
}
  
void Ghost::update(uint32_t time) {
  printf("Ghost::update time %d\n", time);
  Point tile_pt = tile(location);
  uint32_t flags = map.get_flags(tile_pt);
  float vector_multiplier = 1.0f;

  // Adjust speed for being in WARP zone.
  if (flags & entityType::WARP) {
    vector_multiplier = 0.5f;
  }

  if (location.x % 16 > 0 || location.y % 16 > 0) {
    printf("Ghost::update liminal, moving.\n");
    switch (direction) {
      case Button::DPAD_LEFT:
        location.x -= (1.0f * vector_multiplier);
        break;
      case Button::DPAD_RIGHT:
        location.x += (1.0f * vector_multiplier);
        break;
      case Button::DPAD_UP:
        location.y -= (1.0f * vector_multiplier);
        break;
      case Button::DPAD_DOWN:
        location.y += (1.0f * vector_multiplier);
        break;
    }
    return;
  }


  // TODO We are in a PORTAL.
  if (flags & entityType::PORTAL) {
    printf("Ghost::update portal %d\n", state);
    state |= ghostState::GH_PORTAL;
    printf("Ghost::update portal %d\n", state);
    return;
  }

  // Setup
  moving_to = entityType::NOTHING;
  Rect bounds_lr;
  target = player.location;

  // If we're at a junction point choose a new direction.
  if (flags & entityType::JUNCTION) {
    desired_direction = direction_to_target();
  }

  // If our desired path doesn't match our current one try to change.
  if (desired_direction != direction) {
    auto dirSearch = dirToVector.find(desired_direction);
    if (dirSearch == dirToVector.end()) {
      printf("Freezing ghost, no dir mapping.\n");
      return;
    }
    Vec2 desired_movement = dirSearch->second;
    bounds_lr = center(location + desired_movement);
    map.tiles_in_rect(bounds_lr, collision_detection);
    if (moving_to == entityType::NOTHING) {
      direction = desired_direction;
    }
  } else {
    // Otherwise try carrying on.
    moving_to = entityType::NOTHING;
    auto dirSearch = dirToVector.find(direction);
    if (dirSearch == dirToVector.end()) {
      printf("Freezing ghost, no dir mapping.\n");
      return;
    }
    Vec2 movement = dirSearch->second;
    bounds_lr = center(location + movement);
    printf("Can we move into %d:%d %d:%d.\n", bounds_lr.x, bounds_lr.y, bounds_lr.w, bounds_lr.h);
    map.tiles_in_rect(bounds_lr, collision_detection);
  }
  
  // Still can't move, try turning a corner.
  if (moving_to == entityType::WALL && !(flags & entityType::JUNCTION)) {
    uint32_t inverted_direction = invertDirection();
    Point stuck_at = tile(location);
    uint32_t index = stuck_at.y * level_width + stuck_at.x;
    auto search = mapOfJunctions.find(index);
    if (search == mapOfJunctions.end()) {
      printf("No mapping in map of junctions.\n");
      return;
    }

    for (uint32_t corner : search->second) {
      if (corner == inverted_direction) {
        printf("Ghost::update corner refusing to u-turn.\n");
        continue;
      }
      auto dirSearch = dirToVector.find(corner);
      if (dirSearch == dirToVector.end()) {
        return;
      }
      Vec2 vector = dirSearch->second;

      moving_to = entityType::NOTHING;

      map.tiles_in_rect(center(location + vector), collision_detection);
      if (moving_to == entityType::NOTHING) {
        desired_direction = corner;
        break;
      }
    }
    // We haven't reached the corner yet, we've just made a choice.
    return;
  }

  // Set Movement for current direction.
  switch (direction) {
    case Button::DPAD_LEFT:
      location.x -= (1.0f * vector_multiplier);
      break;
    case Button::DPAD_RIGHT:
      printf("Ghost::update pre  loc x %d\n", location.x);
      location.x += (1.0f * vector_multiplier);
      printf("Ghost::update post loc x %d\n", location.x);
      break;
    case Button::DPAD_UP:
      location.y -= (1.0f * vector_multiplier);
      break;
    case Button::DPAD_DOWN:
      location.y += (1.0f * vector_multiplier);
      break;
  }
}

void Ghost::render() {
  if (state & ghostState::GH_PORTAL) {
    printf("Ghost::render in portal.\n");
    return;
  }
    
  printf("Ghost::render %d:%d\n", location.x, location.y);
  screen.sprite(ghostAnims[sprite], world_to_screen(location));
}