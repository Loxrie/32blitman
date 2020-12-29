#include "ghosts.hpp"
#include "ghost.hpp"
#include "main.hpp"
#include "const.hpp"

using namespace blit;

void Blinky::init() {
  ghostAnims[0] = Rect(0,4,2,2);
  ghostAnims[1] = Rect(2,4,2,2);
  ghostAnims[2] = Rect(4,4,2,2);
  ghostAnims[3] = Rect(6,4,2,2);
  ghostAnims[4] = Rect(8,4,2,2);
  ghostAnims[5] = Rect(10,4,2,2);
  ghostAnims[6] = Rect(12,4,2,2);
  ghostAnims[7] = Rect(14,4,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);

  // When power pill running low sub this in for animation 9.
  //   ghostAnims[9] = Rect(10,12,2,2);

  target_offset = 0;
  location = Point((19*8) + 4,15*8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;
  state = ghostState::CHASE;
}

Blinky::Blinky() {
  name = "Blinky";

  init();
}

void Pinky::init() {
  ghostAnims[0] = Rect(0,6,2,2);
  ghostAnims[1] = Rect(2,6,2,2);
  ghostAnims[2] = Rect(4,6,2,2);
  ghostAnims[3] = Rect(6,6,2,2);
  ghostAnims[4] = Rect(8,6,2,2);
  ghostAnims[5] = Rect(10,6,2,2);
  ghostAnims[6] = Rect(12,6,2,2);
  ghostAnims[7] = Rect(14,6,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(14,12,2,2);
  ghostAnims[10] = Rect(12,12,2,2);

  target_offset = 4;
  location = Point((18*8),18*8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::CHASE | ghostState::RESTING | ghostState::LEAVING;
}

Pinky::Pinky() {
  name = "Pinky";

  init();
}

void Inky::init() {
  ghostAnims[0] = Rect(0,8,2,2);
  ghostAnims[1] = Rect(2,8,2,2);
  ghostAnims[2] = Rect(4,8,2,2);
  ghostAnims[3] = Rect(6,8,2,2);
  ghostAnims[4] = Rect(8,8,2,2);
  ghostAnims[5] = Rect(10,8,2,2);
  ghostAnims[6] = Rect(12,8,2,2);
  ghostAnims[7] = Rect(14,8,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);

  target_offset = 2;
  location = Point((19*8) + 4,18*8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::CHASE | ghostState::RESTING;
}

Inky::Inky() {
  name = "Inky";
  
  init();
}

void Clyde::init() {
  ghostAnims[0] = Rect(0,10,2,2);
  ghostAnims[1] = Rect(2,10,2,2);
  ghostAnims[2] = Rect(4,10,2,2);
  ghostAnims[3] = Rect(6,10,2,2);
  ghostAnims[4] = Rect(8,10,2,2);
  ghostAnims[5] = Rect(10,10,2,2);
  ghostAnims[6] = Rect(12,10,2,2);
  ghostAnims[7] = Rect(14,10,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);
  
  // Wrong but it'll do for now.
  target_offset = -4;
  location = Point((21*8),18*8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::CHASE | ghostState::RESTING;
}

Clyde::Clyde() {
  name = "Clyde";

  init();
}