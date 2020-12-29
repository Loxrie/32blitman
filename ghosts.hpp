
#include "32blit.hpp"
#include "const.hpp"
#include "ghost.hpp"

#ifndef GHOSTS_H
#define GHOSTS_H

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