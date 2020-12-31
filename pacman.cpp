#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "assets.hpp"

using namespace blit;

Rect pacmanAnims[16] = {
  Rect(6,12,2,2),
  Rect(4,12,2,2),
  Rect(2,12,2,2),
  Rect(0,12,2,2)
};

Pacman::Pacman() {
  size = Size(15,15);

  lives = 4;
  score = 0;

  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
      this->moving_to = entityType::WALL;
    }
  };
  init(level_data[0]);
}

void Pacman::init(LevelData ld) {
  location = Vec2( (19 * 8) + 4, 27 * 8);
  movement = Vec2(0,0);
  direction = 0;

  speed = ld.pacman_speed;
  dots_speed = ld.pacman_dots_speed;
  fright_speed = ld.pacman_fright_speed;
  fright_dots_speed = ld.pacman_fright_dots_speed;

  power = 0;
  sprite = 0;
  desired_movement = movement;
  desired_direction = direction;
}

bool Pacman::is_pilled_up() {
  return power;
}

void Pacman::animate() {
  bool animDirection = animation_direction;
  switch (sprite) {
    case 0: 
      sprite = 1;
      animation_direction = true;
      break;
    case 1:
      sprite = (animDirection) ? 2 : 0;
      break;
    case 2:
      sprite = (animDirection) ? 3 : 1;
      break;
    case 3:
      sprite = 2;
      animation_direction = false;
      break;
  }
  if (sprite > 15) {
    sprite = 0;
  }
}

void Pacman::update(uint32_t time) {
  static uint32_t last_update = 0;
  float c_speed = speed;
  moving_to = entityType::NOTHING;
  Rect bounds_lr;

  Point tile_pt = tile(location);
  uint32_t flags = map.get_flags(tile_pt);

  if (power)
    c_speed = fright_speed;

  if (level_get(tile_pt) == entityType::PILL) {
    c_speed = dots_speed;
    if (power)
      c_speed = fright_dots_speed;
    score += 10;
    pills_eaten++;
    pills_eaten_this_life++;
    level_set(tile_pt, entityType::NOTHING);
  }

  if (level_get(tile_pt) == entityType::POWER) {
    printf("Pacman::update ate power pill.\n");
    power = 1;
    score += 50;
    pills_eaten++;
    pills_eaten_this_life++;
    level_set(tile_pt, entityType::NOTHING);
    start_power_timer();
    Ghosts::set_state(ghostState::FRIGHTENED);
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
  if (time_passed > 10 / c_speed && moving_to == entityType::NOTHING) {
    last_update = time;
    location += movement;
  }

}

void Pacman::render() {
  uint32_t t = SpriteTransform::NONE;
  switch(direction) {
    case Button::DPAD_RIGHT:
      t = SpriteTransform::R180;
      break;
    case Button::DPAD_UP:
      t = SpriteTransform::R90;
      break;
    case Button::DPAD_DOWN:
      t = SpriteTransform::R270;
      break;
  }

  screen.sprite(pacmanAnims[sprite], world_to_screen(location), t);
}