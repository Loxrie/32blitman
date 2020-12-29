#ifndef CONST_H
#define CONST_H

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

const bool debug_logging = false;

static std::map<uint32_t, blit::Vec2> dirToVector = {
  { blit::Button::DPAD_LEFT, blit::Vec2(-8, 0) },
  { blit::Button::DPAD_RIGHT, blit::Vec2(8, 0) },
  { blit::Button::DPAD_UP, blit::Vec2(0, -8) },
  { blit::Button::DPAD_DOWN, blit::Vec2(0, 8) }
};

static std::map<uint32_t, std::vector<uint32_t>> mapOfJunctions = {
  { 327, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 332, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 338, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 341, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 347, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 352, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 583, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 588, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 591, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 594, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 597, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 600, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 603, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 608, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 775, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 780, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 783, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 786, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 789, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 792, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } },
  { 795, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 800, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } },
  { 975, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 978, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT } },
  { 981, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT } },
  { 984, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 1164, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1167, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1176, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1179, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1359, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1368, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1543, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1548, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1551, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1554, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 1557, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1560, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1563, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1568, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 1735, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1737, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 1740, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1743, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1746, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1749, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1752, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1755, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN,  blit::Button::DPAD_UP } },
  { 1758, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1760, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } },
  { 1927, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1929, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1932, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } },
  { 1935, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1938, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 1941, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_DOWN } },
  { 1944, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } },
  { 1947, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1950, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 1952, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_DOWN } },
  { 2119, { blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 2130, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 2133, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_RIGHT,  blit::Button::DPAD_UP } },
  { 2144, { blit::Button::DPAD_LEFT,  blit::Button::DPAD_UP } }
};

enum entityType {
  NOTHING = 0,
  WALL = 1,
  PILL = 2,
  POWER = 4,
  JUNCTION = 16, // 62 Normal junction. For Ghosts.
  WARP = 32,
  PORTAL = 64
};

enum ghostState {
  CHASE = 1,
  SCATTER = 2,
  FRIGTENED = 4,
  EATEN = 8,
  GH_WARP = 16,
  GH_PORTAL = 32
};

#endif