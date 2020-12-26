#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);
  location = Vec2((19*8)+4,15*8);
  movement = Vec2(-1,0);
  target = Vec2(player.location);
  direction = Button::DPAD_LEFT;
  desired_movement = movement;
  desired_direction = direction;

  sprite = 0;
}

uint32_t Ghost::target_to_dpad() {
  Vec2 vector = target - location;
  if (vector.x < 0 && abs(vector.x) > abs(vector.y)) {
    return Button::DPAD_LEFT;
  } else if (vector.x > 0 && abs(vector.x) > abs(vector.y)) {
    return Button::DPAD_RIGHT;
  } else if (vector.y > 0 && abs(vector.y) > abs(vector.x)) {
    return Button::DPAD_DOWN;
  }
  return Button::DPAD_UP;
}
  
void Ghost::update(uint32_t time) {
  target = player.location;
  desired_direction = target_to_dpad();

  switch(desired_direction) {
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
}