#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

struct Ghost {
  blit::Size size;
  blit::Point location;
  blit::Vec2 movement;
  blit::Vec2 target;
  uint32_t direction;

  blit::Vec2 desired_movement;
  bool decision_made;
  entityType moving_to;

  uint8_t sprite;

  Ghost();
  blit::Vec2 vector_to_target();
  void update(uint32_t time);

  std::function<void(blit::Point)> collision_detection;
};

#endif