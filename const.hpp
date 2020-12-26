#ifndef CONST_H
#define CONST_H

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

const bool debug_logging = false;

enum entityType {
  NOTHING = 0x00,
  WALL = 0x01,
  PILL = 0x02,
  POWER = 0x03,
  GHOST = 0x04,
};

#endif