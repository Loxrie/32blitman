#include "32blit.hpp"
#include "const.hpp"

#ifndef MAIN_H
#define MAIN_H

#include "pacman.hpp"
#include "ghost.hpp"

extern Pacman player;
extern Ghost ghost;

extern blit::Map map;

blit::Rect feet(blit::Vec2 location, blit::Size size);
blit::Point world_to_screen(blit::Point point);
blit::Point screen_to_world(blit::Point point);
blit::Point tile(blit::Point point);
void draw_layer(blit::MapLayer &layer);
void draw_layer(blit::MapLayer &layer, int32_t offset);

#endif