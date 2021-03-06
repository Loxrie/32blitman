
#include "32blit.hpp"
#include "const.hpp"
#include "ghost.hpp"

#ifndef GHOSTS_H
#define GHOSTS_H

extern std::vector<Ghost *> ghosts;

extern std::vector<int32_t> getCycleTimes(int32_t current_level);

// Ghost management functions.
struct Ghosts {
  static void init(uint8_t current_level);
  static void move_start();

  static void set_state(uint8_t s);
  static void clear_state(uint8_t s);
  static void power_pill_eaten();

  // Returns true if a ghost ate pacman.
  static bool update(uint32_t t);
};

struct Blinky : Ghost {
  Blinky();

  void init(LevelData ld);
  blit::Point get_target() override;
};

struct Pinky : Ghost {
  Pinky();

  void init(LevelData ld);
  blit::Point get_target() override;
};

struct Inky : Ghost {
  Inky();

  void init(LevelData ld);
  blit::Point get_target() override;
};

struct Clyde : Ghost {
  Clyde();

  void init(LevelData ld);
  blit::Point get_target() override;
};

#endif