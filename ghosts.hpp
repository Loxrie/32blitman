
#include "32blit.hpp"
#include "const.hpp"
#include "ghost.hpp"

#ifndef GHOSTS_H
#define GHOSTS_H

struct Blinky : Ghost {
  Blinky();

  void render();
};

struct Pinky : Ghost {
  Pinky();
};

struct Inky : Ghost {
  Inky();
};

struct Clyde : Ghost {
  Clyde();
};

#endif