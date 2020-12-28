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

static std::map<uint32_t, std::vector<blit::Vec2>> mapOfJunctions = {
  { 327, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 332, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 338, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 341, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 347, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 352, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 583, { blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 588, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 591, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 594, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 597, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 600, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 603, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 608, { blit::Vec2(-1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 775, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 780, { blit::Vec2(-1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 783, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 786, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 789, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 792, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } },
  { 795, { blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 800, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } },
  { 975, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 978, { blit::Vec2(-1,0),  blit::Vec2(1,0) } },
  { 981, { blit::Vec2(-1,0),  blit::Vec2(1,0) } },
  { 984, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 1164, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1167, { blit::Vec2(-1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1176, { blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1179, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1359, { blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1368, { blit::Vec2(-1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1543, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1548, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1551, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1554, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 1557, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1560, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1563, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1568, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 1735, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1737, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 1740, { blit::Vec2(1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1743, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1746, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1749, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1752, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1755, { blit::Vec2(-1,0),  blit::Vec2(0,1),  blit::Vec2(0,-1) } },
  { 1758, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1760, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } },
  { 1927, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1929, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1932, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } },
  { 1935, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1938, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 1941, { blit::Vec2(1,0),  blit::Vec2(0,1) } },
  { 1944, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } },
  { 1947, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1950, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 1952, { blit::Vec2(-1,0),  blit::Vec2(0,1) } },
  { 2119, { blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 2130, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 2133, { blit::Vec2(-1,0),  blit::Vec2(1,0),  blit::Vec2(0,-1) } },
  { 2144, { blit::Vec2(-1,0),  blit::Vec2(0,-1) } }
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