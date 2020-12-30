
#include "32blit.hpp"
#include "const.hpp"
#include "ghost.hpp"

#ifndef GHOSTS_H
#define GHOSTS_H

extern int8_t blinky_cycle_index;
extern int8_t pinky_cycle_index;
extern int8_t inky_cycle_index;
extern int8_t clyde_cycle_index;

struct Blinky : Ghost {
  Blinky();

  void init();
};

struct Pinky : Ghost {
  Pinky();

  void init();
};

struct Inky : Ghost {
  Inky();

  void init();
};

struct Clyde : Ghost {
  Clyde();

  void init();
};

#endif