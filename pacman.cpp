#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "ghost.hpp"
#include "assets.hpp"

using namespace blit;

Pacman::Pacman() {
  size = Size(16,16);
  location = Vec2((19*8)+4,27*8);
  movement = Vec2(0,0);
  direction = 0;
  desired_movement = movement;
  desired_direction = direction;

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
    printf("Warning sprite %d out of range.", sprite);
    sprite = 0;
  }
}

// std::function<void(Point)> Player::collision_detection = [this](Point tile_pt) -> void {
//   if(this->moving_to == entityType::NOTHING && map.has_flag(tile_pt, entityType::WALL)) {
//     this->moving_to = entityType::WALL;
//   }
// };

void Pacman::update(uint32_t time) {
  Vec2 t_location = Vec2(location.x,location.y);
  moving_to = entityType::NOTHING;
  Rect bounds_lr;

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
  t_location += desired_movement;
  bounds_lr = feet(t_location, size);
  map.tiles_in_rect(bounds_lr, collision_detection);

  // Try existing direction.
  if (moving_to == entityType::WALL) {
    // Continue on.
    moving_to = entityType::NOTHING;
    t_location = location + movement;
    bounds_lr = feet(t_location, size);
    map.tiles_in_rect(bounds_lr, collision_detection);
  } else {
    movement = desired_movement;
    direction = desired_direction;
  }

  if (moving_to == entityType::NOTHING) {
    location = t_location;
  }

  // this->debug_bounds = { 
  //   Point(bounds_lr.x, bounds_lr.y), 
  //   Point(bounds_lr.x + bounds_lr.w, bounds_lr.y), 
  //   Point(bounds_lr.x + bounds_lr.w, bounds_lr.y + bounds_lr.h),
  //   Point(bounds_lr.x, bounds_lr.y + bounds_lr.h)
  // };
}
