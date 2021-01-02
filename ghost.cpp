#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);

  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  scatter_target = Point( 9 * 8, 1 * 8);
  speed = 0.0f;
  last_update = 0;
  
  sprite = 0;
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
  return Rect(pos.x + 4, pos.y + 4,size.h/2, size.w/2);
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
  bool frightened = (state & ghostState::FRIGHTENED);
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

  // In chase state these indexes can't move up.
  // 978, 981, 1746, 1749
  
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

    if (!frightened && exit == Button::DPAD_UP && (index == 978 || index == 981 || index == 1746 || index == 1749)) {
      continue;
    }

    Vec2 vector;
    switch (exit) {
      case Button::DPAD_LEFT:
        vector = Vec2(-1,0);
        break;
      case Button::DPAD_RIGHT:
        vector = Vec2(1,0);
        break;
      case Button::DPAD_UP:
        vector = Vec2(0,-1);
        break;
      case Button::DPAD_DOWN:
        vector = Vec2(0,1);
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
    move_cycle_timer.pause();
  }
  state |= s;
}

void Ghost::clear_state(uint8_t s) {
  if ((state & ghostState::FRIGHTENED)
      && (s & ghostState::FRIGHTENED)
      && move_cycle_timer.is_paused())
  {
    move_cycle_timer.start();
  }
  state &= ~ s;
}

void Ghost::handle_house() {
  if (state & ghostState::EATEN && location == ghost_house_entrance) {
    direction = DPAD_DOWN;
    set_state(ghostState::ARRIVING);
  }

  if (state & ghostState::LEAVING) {
    if (location == ghost_house_exit) {
      direction = Button::DPAD_UP;
      clear_state(ghostState::RESTING);
    } else if (location == ghost_house_exit_l) {
      direction = Button::DPAD_RIGHT;
    } else if (location == ghost_house_exit_r) {
      direction = Button::DPAD_LEFT;
    }
  }

  if (state & ghostState::LEAVING && location == ghost_house_entrance) {
    direction = Button::DPAD_LEFT;
    clear_state(ghostState::LEAVING);
  }

  if (state & ghostState::EATEN && location == ghost_house_exit) {
    clear_state((ghostState::EATEN | ghostState::ARRIVING | ghostState::FRIGHTENED));
    set_state(ghostState::LEAVING);
  }
  
  if (state & ghostState::RESTING && ghost_house.intersects(Rect(location, Size(8,8)))) {
    if (location.y == 17 * 8 + 4) {
      direction = Button::DPAD_DOWN;
    } else if (location.y == 18 * 8 + 4) {
      direction = Button::DPAD_UP;
    }
  }
}

void Ghost::update(uint32_t time) {
  Point tile_pt = tile(location);
  uint32_t flags = level_get(tile_pt);

  float c_speed = speed;

  handle_house();
  
  // Adjust speed for being in WARP zone.
  if (flags & entityType::TUNNEL || state & ghostState::RESTING || state & ghostState::ARRIVING || state & ghostState::LEAVING) {
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
  moving_to = entityType::WALL;
  Rect bounds_lr;

  Point target = player->location;
  // If we're at a junction point choose a new direction.
  bool junction = flags & entityType::JUNCTION;
  bool chase = (state & ghostState::CHASE) && (state & ghostState::FRIGHTENED) == 0;
  bool scatter = (state & ghostState::SCATTER) && (state & ghostState::FRIGHTENED) == 0;
  if (junction && (state & ghostState::EATEN)) {
    direction = direction_to_target(ghost_house_entrance);
  } else if (junction && scatter) {
    direction = direction_to_target(scatter_target);
  } else if (junction && chase) {
    direction = direction_to_target(target);
  } else if (junction && (state & ghostState::FRIGHTENED)) {
    direction = random_direction();
  }

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