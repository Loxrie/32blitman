#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "assets.hpp"

using namespace blit;

Rect pacmanAnims[4] = {
  Rect(6,10,2,2),
  Rect(4,10,2,2),
  Rect(2,10,2,2),
  Rect(0,10,2,2),
};

Rect pacmanDeathAnim[8] = {
  Rect(6,10,2,2),
  Rect(4,10,2,2),
  Rect(2,10,2,2),
  Rect(0,10,2,2),
  Rect(8,10,2,2),
  Rect(10,10,2,2),
  Rect(12,10,2,2),
  Rect(14,10,2,2)
};

Pacman::Pacman() {
  size = Size(15,15);

  lives = 4;
  score = 0;

  init(level_data[0]);
}

void Pacman::init(LevelData ld) {
  location = Point( 19 * 8 + 4, 27 * 8);
  movement = Vec2(-1, 0);
  direction = 1;
  desired_direction = direction;

  speed = ld.pacman_speed;
  dots_speed = ld.pacman_dots_speed;
  fright_speed = ld.pacman_fright_speed;
  fright_dots_speed = ld.pacman_fright_dots_speed;

  power = 0;
  sprite = 0;
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
  uint8_t moving_to = entityType::WALL;
  float c_speed = speed;

  Point tile_pt = tile(location);
  uint8_t flags = level_get(tile_pt);
  if (power)
    c_speed = fright_speed;

  if (flags & entityType::PILL) {
    c_speed = dots_speed;
    if (power)
      c_speed = fright_dots_speed;
    score += 10;
    pills_eaten++;
    pills_eaten_this_life++;
    level_set(tile_pt, (flags & entityType::JUNCTION) ? 44 : 0);
  }

  if (flags & entityType::POWER) {
    power = 1;
    score += 50;
    pills_eaten++;
    pills_eaten_this_life++;
    level_set(tile_pt, (flags & entityType::JUNCTION) ? 44 : 0);
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
      desired_direction = Button::DPAD_UP;
      desired_movement = Vec2(0,-1);
    } 
    else if(buttons & Button::DPAD_DOWN) {
      desired_direction = Button::DPAD_DOWN;
      desired_movement = Vec2(0,1);
    }
    else if(buttons & Button::DPAD_LEFT) {
      desired_direction = Button::DPAD_LEFT;
      desired_movement = Vec2(-1,0);
    }
    else if(buttons & Button::DPAD_RIGHT) {
      desired_direction = Button::DPAD_RIGHT;
      desired_movement = Vec2(1,0);
    }
  }
  
  // Only check for movement validatity every 8 pixels.
  if (location.x % 8 == 0 && location.y % 8 == 0) {
    if (desired_direction != direction) {
      moving_to = level_get(tile_pt + desired_movement);
      if ((moving_to & entityType::WALL) == 0) {
        movement = desired_movement;
        direction = desired_direction;
      }
    }
    
    if (moving_to & entityType::WALL) {
      moving_to = level_get(tile_pt + movement);
      if (moving_to & entityType::WALL) {
        movement = Vec2(0,0);
      }
    }
  }
  
  uint32_t time_passed = time - last_update;
  if (time_passed > 10 / c_speed) {
    last_update = time;
    location += movement;
  }
}

void Pacman::render() {
  uint8_t t = SpriteTransform::NONE;
  switch(direction) {
    case Button::DPAD_RIGHT:
      t = SpriteTransform::HORIZONTAL;
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

void Pacman::render_lives() {
  screen.pen = Pen(255,255,255);
  screen.text("Lives ", minimal_font, Point(30, screen_height - 12));
  for(auto i = 0; i<player->lives; i++) {
    screen.sprite(pacmanAnims[3], Point(60 + (i*16) + 2, screen_height - 18));
  }
}