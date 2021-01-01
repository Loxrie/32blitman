#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);

  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  scatter_target = Point( (9 * 8), 1 * 8);
  speed = 0.0f;
  last_update = 0;
  
  sprite = 0;
  
  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
        this->moving_to = entityType::WALL;
    }
  };
}

// This is called once every 100ms or so.
void Ghost::animate(uint32_t t) {
  if (state & ghostState::EATEN) {
    sprite = (sprite % 2) ? 37 : 36;
  } else if (state & ghostState::FRIGHTENED) {
    if (power_timer.is_running() 
        && (power_timer.duration - (t - power_timer.started)) < level_data[current_level].fright_warning_time)
    {
      sprite = (sprite % 2) ? 34 : 35;
    } else {
      sprite = (sprite % 2) ? 32 : 33;
    }
  } else {
    switch(direction) {
      case Button::DPAD_LEFT:
        sprite = (sprite % 2) ? anim_offset : anim_offset + 1;
        break;
      case Button::DPAD_RIGHT:
        sprite = (sprite % 2) ? anim_offset + 2 : anim_offset + 3;
        break;
      case Button::DPAD_UP:
        sprite = (sprite % 2) ? anim_offset + 4 : anim_offset + 5;
        break;
      case Button::DPAD_DOWN:
        sprite = (sprite % 2) ? anim_offset + 6 : anim_offset + 7;
        break;
    }
  }
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

uint32_t Ghost::direction_to_target(Point target) {
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
  target_tile = tile(target);

  switch(player->direction) {
    case Button::DPAD_LEFT:
      target_tile.x -= target_offset;
      break;
    case Button::DPAD_RIGHT:
      target_tile.x += target_offset;
      break;
    case Button::DPAD_UP:
      target_tile.y -= target_offset;
      break;
    case Button::DPAD_DOWN:
      target_tile.y += target_offset;
      break;
  }
  
  uint32_t inverted_direction = invertDirection();
  uint32_t index = decision_tile.y * level_width + decision_tile.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    return 0;
  }

  float min_distance = 600.f;
  uint32_t min_direction = 0;
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
    float distance = fabs(Vec2(target_tile.x - source_tile.x, target_tile.y - source_tile.y).length());
    if (distance < min_distance) {
      min_distance = distance;
      min_direction = exit;
    }
  }

  return min_direction;
}

uint32_t Ghost::random_direction() {
  int bloop = rand();
  uint32_t inverted_direction = invertDirection();
  uint32_t random_direction = 0;
  Point stuck_at = tile(location);
  uint32_t index = stuck_at.y * level_width + stuck_at.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    return 0;
  }

  return search->second[bloop % search->second.size()];
}

bool Ghost::edible() {
  return ((state & ghostState::EATEN) == 0 && state & ghostState::FRIGHTENED && player->is_pilled_up());
}

bool Ghost::eaten() {
  return state & ghostState::EATEN;
}

void Ghost::set_move_state(ghostState s) {
  clear_state(ghostState::CHASE | ghostState::SCATTER);
  set_state(s);
}

void Ghost::set_state(uint8_t s) {
  if ((state & ghostState::FRIGHTENED) == 0 && (s == ghostState::FRIGHTENED)) {
    //printf("%s::set_state pausing move cycle, entering frightened.\n", name.c_str());
    move_cycle_timer.pause();
  }
  state |= s;
}

void Ghost::clear_state(uint8_t s) {
  //printf("%s::clear_state current state %d, flags to clear %d, move cycle %d.\n", name.c_str(), state, s, move_cycle_timer.state);
  if ((state & ghostState::FRIGHTENED)
      && (s & ghostState::FRIGHTENED)
      && move_cycle_timer.is_paused())
  {
    //printf("%s::clear_state resuming move cycle, leaving frightened.\n", name.c_str());
    move_cycle_timer.start();
  }
  state &= ~ s;
}

void Ghost::handle_house() {
  if (state & ghostState::EATEN && location == ghost_house_door) {
    direction = DPAD_DOWN;
    set_state(ghostState::ARRIVING);
  }

  if (state & ghostState::LEAVING && location == ghost_house) {
    direction = Button::DPAD_UP;
    clear_state(ghostState::RESTING);
  }

  if (state & ghostState::LEAVING && location == ghost_house_door) {
    direction = Button::DPAD_LEFT;
    clear_state(ghostState::LEAVING);
  }

  if (state & ghostState::EATEN && location == ghost_house) {
    direction = Button::DPAD_LEFT;
    clear_state((ghostState::EATEN | ghostState::ARRIVING | ghostState::FRIGHTENED));
    set_state(ghostState::LEAVING);
    return;
  }
  
  if (state & ghostState::RESTING) {
    if (location == ghost_house_left) {
      direction = Button::DPAD_RIGHT;
    } else if (location == ghost_house_right) {
      direction = Button::DPAD_LEFT;
    }
  }
}

void Ghost::update(uint32_t time) {
  Point tile_pt = tile(location);
  uint32_t flags = map.get_flags(tile_pt);

  float c_speed = speed;

  // Adjust speed for being in WARP zone.
  if (flags & entityType::WARP) {
    c_speed = tunnel_speed;
  } else if (state & ghostState::FRIGHTENED) {
    c_speed = fright_speed;
  }

  // TODO We are in a PORTAL.
  if (flags & entityType::PORTAL) {
    if (direction == Button::DPAD_LEFT && tile_pt == Point(4,18)) {
      location = Vec2(35 * 8, 18 * 8);
    } else if (direction == Button::DPAD_RIGHT && tile_pt == Point(35, 18)) {
      location = Vec2(4 * 8, 18 * 8);
    }
    tile_pt = tile(location);
    flags = map.get_flags(tile_pt);
  }

  handle_house();

  if (location.x % 8 > 0 
      || location.y % 8 > 0 
      || state & ghostState::ARRIVING 
      || state & ghostState::LEAVING 
      || state & ghostState::RESTING) 
  {
    // Only update every "speed" fraction of frames. Assume 10ms update.
    // So by default we start updating at 10/0.75. 
    // This won't make much diff. for now.  Maybe move 2pixels per tic?
    if (time - last_update > 20 / c_speed) {
      last_update = time;
      switch (direction) {
        case Button::DPAD_LEFT:
          location.x -= 1.0f;
          break;
        case Button::DPAD_RIGHT:
          location.x += 1.0f;
          break;
        case Button::DPAD_UP:
          location.y -= 1.0f;
          break;
        case Button::DPAD_DOWN:
          location.y += 1.0f;
          break;
      }
    }
    return;
  }

  // Setup
  moving_to = entityType::NOTHING;
  Rect bounds_lr;

  Vec2 target = player->location;
  // If we're at a junction point choose a new direction.
  bool junction = flags & entityType::JUNCTION;
  bool chase = (state & ghostState::CHASE) && (state & ghostState::FRIGHTENED) == 0;
  bool scatter = (state & ghostState::SCATTER) && (state & ghostState::FRIGHTENED) == 0;
  if (junction && (state & ghostState::EATEN)) {
    desired_direction = direction_to_target(ghost_house_door);
  } else if (junction && scatter) {
    desired_direction = direction_to_target(scatter_target);
  } else if (junction && chase) {
    desired_direction = direction_to_target(target);
  } else if (junction && (state & ghostState::FRIGHTENED)) {
    desired_direction = random_direction();
  }

  // If our desired path doesn't match our current one try to change.
  if (desired_direction != direction) {
    auto dirSearch = dirToVector.find(desired_direction);
    if (dirSearch == dirToVector.end()) {
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
      return;
    }
    Vec2 movement = dirSearch->second;
    bounds_lr = center(location + movement);
    map.tiles_in_rect(bounds_lr, collision_detection);
  }
  
  // Still can't move, try turning a corner.
  if (moving_to == entityType::WALL && !(flags & entityType::JUNCTION)) {
    uint32_t inverted_direction = invertDirection();
    Point stuck_at = tile(location);
    uint32_t index = stuck_at.y * level_width + stuck_at.x;
    auto search = mapOfJunctions.find(index);
    if (search == mapOfJunctions.end()) {
      return;
    }

    for (uint32_t corner : search->second) {
      if (corner == inverted_direction) {
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
      location.x -= 1.0f;
      break;
    case Button::DPAD_RIGHT:
      location.x += 1.0f;
      break;
    case Button::DPAD_UP:
      location.y -= 1.0f;
      break;
    case Button::DPAD_DOWN:
      location.y += 1.0f;
      break;
  }
}

void Ghost::render() {  
  screen.sprite(ghostAnims[sprite], world_to_screen(location));
}