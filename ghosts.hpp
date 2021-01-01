
#include "32blit.hpp"
#include "const.hpp"
#include "ghost.hpp"

#ifndef GHOSTS_H
#define GHOSTS_H

extern uint8_t blinky_cycle_index;
extern uint8_t pinky_cycle_index;
extern uint8_t inky_cycle_index;
extern uint8_t clyde_cycle_index;

extern std::vector<Ghost *> ghosts;

// Ghost management functions.
struct Ghosts {
  static void init(uint8_t current_level);
  static void move_start();
  static void move_pause();
  static void move_resume();
  static void move_reset_and_pause();

  static void set_state(uint8_t s);
  static void clear_state(uint8_t s);

  // Returns true if a ghost ate pacman.
  static bool update(uint32_t t);
};

struct Blinky : Ghost {
  Blinky();

  void init(LevelData ld);
};

struct Pinky : Ghost {
  Pinky();

  void init(LevelData ld);
};

struct Inky : Ghost {
  Inky();

  void init(LevelData ld);
};

struct Clyde : Ghost {
  Clyde();

  void init(LevelData ld);
};

#endif