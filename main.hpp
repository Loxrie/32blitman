#include "32blit.hpp"
#include "const.hpp"

#ifndef MAIN_H
#define MAIN_H

#include "pacman.hpp"
#include "ghost.hpp"
#include "ghosts.hpp"

extern blit::SpriteSheet *level_sprites;
extern blit::Timer power_timer;

extern uint8_t *pill_data;
extern uint8_t current_level;

extern blit::Map map;

extern Pacman *player;
extern Blinky *blinky;
extern Pinky *pinky;
extern Inky *inky;
extern Clyde *clyde;

extern uint32_t pills_eaten;
extern uint32_t pills_eaten_this_life;

extern bool operator==(blit::Point a, blit::Point b);
extern bool operator!=(blit::Point a, blit::Point b);

entityType level_get(blit::Point p);
void level_set(blit::Point p, entityType e);

void start_power_timer();
blit::Rect footprint(blit::Point pos, blit::Size size);
blit::Point world_to_screen(blit::Point point);
blit::Point screen_to_world(blit::Point point);
blit::Point tile(blit::Point point);
void draw_layer(uint8_t *layer);
void draw_layer(uint8_t *layer, int32_t offset);

void next_level();
void reset_level();
void game_over();

#endif