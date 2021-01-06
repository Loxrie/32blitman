#include "32blit.hpp"
#include "const.hpp"
#include "main.hpp"

#ifndef PACMAN_H
#define PACMAN_H

struct Pacman {

  blit::Size size;
  blit::Point location;
  blit::Point tile;

  uint32_t direction;
  uint32_t desired_direction;

  blit::Vec2 movement;
  blit::Vec2 desired_movement;

  float velocity;
  float speed;
  float dots_speed;
  float fright_speed;
  float fright_dots_speed;

  void power_timer_callback(blit::Timer &t);
  blit::Timer power_timer;
  bool power;

  uint8_t sprite;
  bool animation_direction;
  uint32_t score;
  uint32_t level;
  uint32_t lives;

  Pacman();
  void init(LevelData ld);
  bool is_pilled_up();

  void animate();
  void update(uint32_t time);
  void render();
  void render_lives();
};

#endif
