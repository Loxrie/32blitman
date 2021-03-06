#ifndef CONST_H
#define CONST_H

const uint32_t button_debounce_rate = 120;

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

const uint32_t pills_per_level = 244;

const blit::Point ghost_house_entrance = blit::Point(156, 120); //
const blit::Point ghost_house_exit = blit::Point(156, 144);
// const blit::Point ghost_house_exit_l = blit::Point(140, 144);
// const blit::Point ghost_house_exit_r = blit::Point(172, 144);
const blit::Rect ghost_house = blit::Rect(136, 136, 48, 24);

static std::map<uint32_t, blit::Vec2> dirToVector = {{blit::Button::DPAD_LEFT, blit::Vec2(-8, 0)},
                                                     {blit::Button::DPAD_RIGHT, blit::Vec2(8, 0)},
                                                     {blit::Button::DPAD_UP, blit::Vec2(0, -8)},
                                                     {blit::Button::DPAD_DOWN, blit::Vec2(0, 8)}};

// There is a direction preference for ghosts, UP > LEFT > DOWN > RIGHT.
// SO if we reverse that order and there's a tie our preferred direction should
// win.
static std::map<uint32_t, std::vector<uint32_t>> mapOfJunctions = {
    {327, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {332, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {338, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {341, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {347, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {352, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {583, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_UP}},
    {588, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {591, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {594, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {597, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {600, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {603, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {608, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {775, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {780, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {783, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {786, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {789, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {792, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {795, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_UP}},
    {800, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {975, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {978, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {981, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {984, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1164, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1167, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1176, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_UP}},
    {1179, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1359, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_UP}},
    {1368, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1543, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {1548, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1551, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1554, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1557, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {1560, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1563, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1568, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1735, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {1737, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1740, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_UP}},
    {1743, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1746, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1749, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1752, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1755, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1758, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {1760, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1927, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {1929, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1932, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1935, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {1938, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {1941, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_DOWN}},
    {1944, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1947, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {1950, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {1952, {blit::Button::DPAD_DOWN, blit::Button::DPAD_LEFT}},
    {2119, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_UP}},
    {2130, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {2133, {blit::Button::DPAD_RIGHT, blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}},
    {2144, {blit::Button::DPAD_LEFT, blit::Button::DPAD_UP}}};

enum entityType {
  NOTHING = 0,
  WALL = 1,
  JUNCTION = 2, // Normal junction. For Ghosts.
  TUNNEL = 4,
  PORTAL = 8,
  PILL = 16,
  POWER = 32,
  CORNER = 64
};

const uint8_t mappings[51] = {entityType::NOTHING,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::WALL,
                              entityType::TUNNEL,
                              entityType::PORTAL,
                              entityType::JUNCTION,
                              entityType::JUNCTION | entityType::PILL,
                              entityType::PILL,
                              entityType::CORNER | entityType::POWER,
                              entityType::POWER,
                              entityType::CORNER,
                              entityType::CORNER | entityType::PILL};

enum ghostState { CHASE = 1, SCATTER = 2, FRIGHTENED = 4, EATEN = 8, RESTING = 16, ARRIVING = 32, LEAVING = 64 };

struct LevelData {
  // Sprite for bonus fruit.
  uint32_t bonus_image;
  // Points for bonus fruit.
  uint32_t bonus_points;

  // As a percentage of an unknown max.
  float pacman_speed;
  float pacman_dots_speed;
  float pacman_fright_speed;
  float pacman_fright_dots_speed;
  // As a percentage of an unknown max.
  float ghost_speed;
  float ghost_tunnel_speed;
  float ghost_fright_speed;

  uint32_t fright_time; // in ms.
  // ms of warnings before ghosts revert.  Was "no. of flashes" og
  // We animate roughly every 100ms for ghosts, so 1000 should be 5 complete
  // flashes.
  uint32_t fright_warning_time;
};

const std::array<blit::Rect, 7> fruit_mapping = {
    blit::Rect(0, 14, 2, 2), blit::Rect(2, 14, 2, 2),  blit::Rect(4, 14, 2, 2), blit::Rect(6, 14, 2, 2),
    blit::Rect(8, 14, 2, 2), blit::Rect(10, 14, 2, 2), blit::Rect(12, 14, 2, 2)};

// bi, bp, pms,  pmds,  pmfs,  pmfds,  gs,   gts,   gfs,   ft,   fwt
static std::array<LevelData, 21> level_data = {
    (LevelData){0, 100, 0.55f, 0.46f, 0.65f, 0.54f, 0.50f, 0.15f, 0.30f, 6000, 1100},  // 1
    (LevelData){1, 300, 0.65f, 0.54f, 0.70f, 0.58f, 0.60f, 0.20f, 0.30f, 5000, 1100},  // 2
    (LevelData){2, 500, 0.65f, 0.54f, 0.70f, 0.58f, 0.60f, 0.20f, 0.30f, 4000, 1100},  // 3
    (LevelData){2, 500, 0.65f, 0.54f, 0.70f, 0.58f, 0.60f, 0.20f, 0.30f, 3000, 1100},  // 4
    (LevelData){3, 700, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 2000, 1100},  // 5
    (LevelData){3, 700, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 5000, 1100},  // 6
    (LevelData){4, 1000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 2000, 1100}, // 7
    (LevelData){4, 1000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 2000, 1100}, // 8
    (LevelData){5, 2000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 1000, 700},  // 9
    //(LevelData) bi, bp, pms,  pmds,  pmfs,  pmfds,  gs,   gts,   gfs,   ft,
    // fwt
    (LevelData){5, 2000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 5000, 1100}, // 10
    (LevelData){6, 3000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 2000, 1100}, // 11
    (LevelData){6, 3000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 1000, 700},  // 12
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 1000, 700},  // 13
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 3000, 1100}, // 14
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 1000, 700},  // 15
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 0, 0},       // 17
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 1000, 700},  // 18
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 0, 0},       // 19
    (LevelData){7, 5000, 0.75f, 0.62f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 0, 0},       // 20
    (LevelData){7, 5000, 0.65f, 0.54f, 0.75f, 0.62f, 0.70f, 0.25f, 0.35f, 0, 0}        // 21
};

#endif