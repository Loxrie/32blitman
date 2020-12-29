#include "ghosts.hpp"
#include "ghost.hpp"
#include "main.hpp"
#include "const.hpp"

using namespace blit;

Blinky::Blinky() {
  printf("Blinky::Blinky called.\n");
  name = "Blinky";
}

void Blinky::render() {
  printf("%s::render %d:%d\n", name.c_str(), location.x, location.y);    
  screen.sprite(ghostAnims[sprite], world_to_screen(location));
}

Pinky::Pinky() {
  printf("Pinky::Pinky called.\n");
  name = "Pinky";
}

Inky::Inky() {
  printf("Inky::Inky called.\n");
  name = "Inky";
}

Clyde::Clyde() {
  printf("Clyde::Clyde called.\n");
  name = "Clyde";
}