#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

struct Ghost {
  blit::Size size;
  blit::Point location;
  blit::Vec2 target;

  uint32_t direction;
  uint32_t desired_direction;

  blit::Tween tween;

  uint8_t state;

  entityType moving_to;

  uint8_t sprite;

  Ghost();

  uint32_t invertDirection();
  blit::Rect center(blit::Point pos);
  uint32_t direction_to_target();
  void update(uint32_t time);
  void render();

  std::function<void(blit::Point)> collision_detection;
};

#endif