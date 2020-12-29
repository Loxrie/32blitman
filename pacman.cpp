#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "ghost.hpp"
#include "assets.hpp"

using namespace blit;

Pacman::Pacman() {
  size = Size(15,15);
  location = Vec2((19*8)+4,27*8);
  movement = Vec2(0,0);
  direction = 0;
  desired_movement = movement;
  desired_direction = direction;

  speed = 0.8f;
  power_timer = 0;

  sprite = 0;
  lives = 3;
  score = 0;

  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
      this->moving_to = entityType::WALL;
    }
  };
}

void Pacman::anim_player() {
  uint8_t offset = 0;
  switch(direction) {
    case Button::DPAD_LEFT:
      offset = 0;
      break;
    case Button::DPAD_RIGHT:
      offset = 4;
      break;
    case Button::DPAD_UP:
      offset = 8;
      break;
    case Button::DPAD_DOWN:
      offset = 12;
      break;
  }
  bool animDirection = animation_direction;
  switch (sprite % 4) {
    case 0: 
      sprite = offset + 1;
      animation_direction = true;
      break;
    case 1:
      sprite = (animDirection) ? offset + 2 : offset + 0;
      break;
    case 2:
      sprite = (animDirection) ? offset + 3 : offset + 1;
      break;
    case 3:
      sprite = offset + 2;
      animation_direction = false;
      break;
  }
  if (sprite > 15) {
    sprite = 0;
  }
}

void Pacman::update(uint32_t time) {
  static uint32_t last_update = 0;
  moving_to = entityType::NOTHING;
  Rect bounds_lr;

  Point tile_pt = tile(location);
  uint32_t flags = map.get_flags(tile_pt);

  if (flags & entityType::PILL) {
    if (power_timer <= 0)
      speed = 0.71f;
    map.layers["pills"].tiles[tile_pt.y * level_width + tile_pt.x] = 0;
  }

  if (flags & entityType::POWER) {
    speed = 0.90f;
    power_timer = 30 * 1000;
    map.layers["pills"].tiles[tile_pt.y * level_width + tile_pt.x] = 0;
  }


  // TODO We are in a PORTAL.
  if (flags & entityType::PORTAL) {
    if (direction == Button::DPAD_LEFT && tile_pt == Point(4,18)) {
      location = Vec2(35 * 8, 18 * 8);
    } else if (direction == Button::DPAD_RIGHT && tile_pt == Point(35, 18)) {
      location = Vec2(4 * 8, 18 * 8);
    }
  }

  if (buttons.state > 0) {
    // Possible new direction.
    if(buttons & Button::DPAD_UP) {
      desired_movement = Vec2(0,-1);
      desired_direction = Button::DPAD_UP;
    } 
    else if(buttons & Button::DPAD_DOWN) {
      desired_movement = Vec2(0,1);
      desired_direction = Button::DPAD_DOWN;
    }
    else if(buttons & Button::DPAD_LEFT) {
      desired_movement = Vec2(-1,0);
      desired_direction = Button::DPAD_LEFT;
    }
    else if(buttons & Button::DPAD_RIGHT) {
      desired_movement = Vec2(1,0);
      desired_direction = Button::DPAD_RIGHT;
    }
  }

  // See if new input direction is valid.
  bounds_lr = footprint(location + desired_movement, size);
  map.tiles_in_rect(bounds_lr, collision_detection);

  // Try existing direction.
  if (moving_to == entityType::WALL) {
    // Continue on.
    moving_to = entityType::NOTHING;
    bounds_lr = footprint(location + movement, size);
    map.tiles_in_rect(bounds_lr, collision_detection);
  } else {
    movement = desired_movement;
    direction = desired_direction;
  }

  uint32_t time_passed = time - last_update;
  if (time_passed > 10 / speed && moving_to == entityType::NOTHING) {
    printf("Pacman::update last_update %d, speed %f\n", last_update, speed);
    last_update = time;
    location += movement;
    // Should we expire power pill.
    if (power_timer > 0) {
      power_timer -= time_passed;
    }
  }
}
