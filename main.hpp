#include "32blit.hpp"
#include "const.hpp"

#ifndef MAIN_H
#define MAIN_H

#include "pacman.hpp"
#include "ghost.hpp"
#include "ghosts.hpp"

extern blit::Mat3 camera;

extern blit::SpriteSheet *level_sprites;
extern blit::Timer power_timer;

extern uint8_t *level_tiles;
extern uint8_t current_level;

extern std::array<int32_t, 8> cycleTimes;

extern Pacman *player;
extern Blinky *blinky;
extern Pinky *pinky;
extern Inky *inky;
extern Clyde *clyde;

extern uint8_t ghost_train;
extern uint32_t pill_eaten_time;
extern uint32_t pills_eaten;
extern uint32_t pills_eaten_this_life;

extern bool operator==(blit::Point a, blit::Point b);
extern bool operator!=(blit::Point a, blit::Point b);

std::array<int32_t, 8> getCycleTimes();

uint8_t level_get(blit::Point p);
void level_set(blit::Point p, uint8_t e);

void start_power_timer();
blit::Point world_to_screen(blit::Point point);
blit::Point get_tile(blit::Point point);

void next_level();
void reset_level();
void game_over();

#endif