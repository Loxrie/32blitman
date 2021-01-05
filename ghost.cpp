#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);

  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  scatter_target = Point(9 * 8, 1 * 8);
  velocity = 0.0f;
  speed = 0.0f;
  last_update = 0;

  sprite = 0;

  forced_direction_change = false;
}

// This is called once every 100ms or so.
void Ghost::animate(uint32_t t) {
  bool eaten = state & ghostState::EATEN;
  if (!eaten && state & ghostState::FRIGHTENED) {
    if (power_timer.is_running() &&
        (power_timer.duration - (t - power_timer.started)) <
            level_data[current_level].fright_warning_time) {
      sprite = (sprite % 2) ? 34 : 35;
    } else {
      sprite = (sprite % 2) ? 32 : 33;
    }
  } else {
    switch (desired_direction) {
    case Button::DPAD_LEFT:
      sprite = (eaten) ? 36 : (sprite % 2) ? anim_offset : anim_offset + 1;
      break;
    case Button::DPAD_RIGHT:
      sprite = (eaten) ? 37 : (sprite % 2) ? anim_offset + 2 : anim_offset + 3;
      break;
    case Button::DPAD_UP:
      sprite = (eaten) ? 38 : (sprite % 2) ? anim_offset + 4 : anim_offset + 5;
      break;
    case Button::DPAD_DOWN:
      sprite = (eaten) ? 39 : (sprite % 2) ? anim_offset + 6 : anim_offset + 7;
      break;
    }
  }
}

uint32_t Ghost::invertDirection() {
  uint32_t i_direction = 0;
  switch (direction) {
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
  return Rect(pos.x + 4, pos.y + 4, size.h / 2, size.w / 2);
}

uint32_t Ghost::direction_to_target(Point target_tile) {
  /**
   * https://gameinternals.com/understanding-pac-man-ghost-behavior
   *
   * When a decision about which direction to turn is necessary,
   * the choice is made based on which tile adjoining the intersection
   * will put the ghost nearest to its target tile, measured in a straight line.
   * The distance from every possibility to the target tile is measured, and
   * whichever tile is closest to the target will be selected.
   */

  uint32_t where_will_we_go = 0;
  bool frightened = (state & ghostState::FRIGHTENED);

  switch (player->direction) {
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
  uint32_t index = tile.y * level_width + tile.x;
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

    if (!frightened && exit == Button::DPAD_UP &&
        (index == 978 || index == 981 || index == 1746 || index == 1749)) {
      continue;
    }

    Vec2 vector = dirToVector[exit];
    Point source_tile = Point(tile.x + vector.x, tile.y + vector.y);
    float distance =
        fabs(Vec2(target_tile.x - source_tile.x, target_tile.y - source_tile.y)
                 .length());
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
  Point stuck_at = get_tile(location);
  uint32_t index = stuck_at.y * level_width + stuck_at.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    return 0;
  }

  std::vector<uint32_t> remove_the_way_we_came;
  for (uint32_t d : search->second) {
    if (d != inverted_direction) {
      remove_the_way_we_came.push_back(d);
    }
  }
  uint32_t rd = remove_the_way_we_came[bloop % remove_the_way_we_came.size()];
  return rd;
}

bool Ghost::edible() {
  return ((state & (ghostState::EATEN | ghostState::FRIGHTENED)) ==
              ghostState::FRIGHTENED &&
          player->is_pilled_up());
}

bool Ghost::eaten() { return state & ghostState::EATEN; }

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
  if ((state & ghostState::FRIGHTENED) && (s & ghostState::FRIGHTENED) &&
      move_cycle_timer.is_paused()) {
    move_cycle_timer.start();
  }
  state &= ~s;
}

void Ghost::handle_house() {
  if (state & ghostState::EATEN && location == ghost_house_entrance) {
    direction = DPAD_DOWN;
    desired_direction = direction;
    set_state(ghostState::ARRIVING);
    return;
  }

  if (state & ghostState::LEAVING && location == ghost_house_entrance) {
    direction = Button::DPAD_LEFT;
    desired_direction = direction;
    clear_state(ghostState::LEAVING);
    return;
  }

  if (state & ghostState::EATEN && location == ghost_house_exit) {
    clear_state(
        (ghostState::EATEN | ghostState::ARRIVING | ghostState::FRIGHTENED));
    set_state(ghostState::LEAVING);
    return;
  }

  if (state & ghostState::LEAVING) {
    if (location == ghost_house_exit) {
      direction = Button::DPAD_UP;
      desired_direction = direction;
      clear_state(ghostState::RESTING);
      return;
    } else if (location.y == ghost_house_exit.y &&
               location.x < ghost_house_exit.x) {
      direction = Button::DPAD_RIGHT;
      desired_direction = direction;
      return;
    } else if (location.y == ghost_house_exit.y &&
               location.x > ghost_house_exit.x) {
      direction = Button::DPAD_LEFT;
      desired_direction = direction;
      return;
    }
  }

  if (state & ghostState::RESTING &&
      ghost_house.intersects(Rect(location, Size(8, 8)))) {
    if (location.y == 17 * 8 + 4) {
      direction = Button::DPAD_DOWN;
      desired_direction = direction;
    } else if (location.y == 18 * 8 + 4) {
      direction = Button::DPAD_UP;
      desired_direction = direction;
    }
    return;
  }
}

void Ghost::update(uint32_t time) {
  Point tile_pt = get_tile(location);
  uint32_t flags = level_get(tile);

  float c_speed = speed;

  // Only do special ghost house stuff if the ghost is around the house.
  if ((state & house_flags) > 0 || location == ghost_house_entrance) {
    handle_house();
  }

  // Adjust speed for being in WARP zone.
  if ((flags & tunnel_entity_flags) > 0 || (state & house_flags) > 0) {
    c_speed = tunnel_speed;
  } else if (state & ghostState::FRIGHTENED) {
    c_speed = fright_speed;
  } else if (state & ghostState::EATEN) {
    c_speed = 1.0f;
  }

  // We are in a PORTAL.
  if (flags & entityType::PORTAL) {
    if (direction == Button::DPAD_LEFT && tile == Point(4, 18)) {
      location = Vec2(35 * 8, 18 * 8);
    } else if (direction == Button::DPAD_RIGHT && tile == Point(35, 18)) {
      location = Vec2(4 * 8, 18 * 8);
    }
    tile = get_tile(location);
    flags = level_get(tile);
  }

  // If x && y % 8 then change direction to desired direction.
  // This better mimics arcade where directino changes occur when a tile is
  // changed. Which is not necessarily bang on 8 pixels. But preserves the fact
  // we only actually move in the center of an aisle.
  if (location.x % 8 == 0 && location.y % 8 == 0) {
    direction = desired_direction;
  }

  last_update = time;
  velocity += c_speed;
  if (velocity > 1.0f) {
    velocity -= 1.0f;
    switch (direction) {
    case Button::DPAD_LEFT:
      location.x -= 1;
      break;
    case Button::DPAD_RIGHT:
      location.x += 1;
      break;
    case Button::DPAD_UP:
      location.y -= 1;
      break;
    case Button::DPAD_DOWN:
      location.y += 1;
      break;
    }
  }

  tile_pt = get_tile(location);
  if (tile == tile_pt || (state & house_flags) > 0) {
    return;
  }

  tile = tile_pt;
  flags = level_get(tile);

  // If we're at a junction point choose a new direction.
  bool junction = flags & (entityType::JUNCTION | entityType::CORNER);
  bool chase = (state & (ghostState::CHASE | ghostState::FRIGHTENED)) ==
               ghostState::CHASE;
  bool scatter = (state & (ghostState::SCATTER | ghostState::FRIGHTENED)) ==
                 ghostState::SCATTER;
  if (junction) {
    if (forced_direction_change) {
      forced_direction_change = false;
      desired_direction = invertDirection();
    } else if ((state & ghostState::EATEN)) {
      desired_direction = direction_to_target(get_tile(ghost_house_entrance));
    } else if (junction && scatter) {
      desired_direction = direction_to_target(scatter_target);
    } else if (junction && chase) {
      desired_direction = direction_to_target(player->tile);
    } else if (junction && (state & ghostState::FRIGHTENED)) {
      desired_direction = random_direction();
    }
  } else {
    return;
  }
}

void Ghost::render() {
  screen.sprite(ghostAnims[sprite], world_to_screen(location));
  screen.pen = Pen(255, 128, 128);
  screen.pixel(world_to_screen(Point(tile.x * 8 + 4, tile.y * 8 + 4)));
}