#ifndef CONST_H
#define CONST_H

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

const bool debug_logging = false;

enum entityType {
  NOTHING = 0,
  WALL = 1,
  PILL = 2,
  POWER = 4,
  GHOST = 8,
  JUNCTION = 16, // 62 Normal junction. For Ghosts.
  NU_JUNCTION = 32 // 63 No UP junction. For Ghosts.
};

#endif