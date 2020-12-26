#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

struct Ghost {
  blit::Size size;
  blit::Vec2 location;
  blit::Vec2 movement;
  blit::Vec2 target;
  uint32_t direction;

  blit::Vec2 desired_movement;
  uint32_t desired_direction;

  uint8_t sprite;

  Ghost();
  uint32_t target_to_dpad();
  void update(uint32_t time);
};

#endif