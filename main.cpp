#include "main.hpp"
#include "assets.hpp"
#include "ghost.hpp"
#include "ghosts.hpp"
#include "pacman.hpp"
#include <time.h>

using namespace blit;

uint32_t last_time = 0;

Mat3 camera;
Mat3 inverse_camera;

uint8_t *level_tiles;
std::array<int32_t, 8> cycleTimes;

TileMap *level;

SpriteSheet *level_sprites;

/**
 * Timers and callbacks.
 */
Timer timer_level_animate;
Timer power_timer;
Timer house_timer;
Timer fruit_timer;

bool bodge_ghost_animation = false;

/**
 * Ghost house state
 */
bool lives_not_lost_this_level = true;
uint8_t inky_dot_limit = 30;
uint8_t clyde_dot_limit = 90;

/**
 * Sprite Managers/Characters
 */
Pacman *player;
Blinky *blinky;
Pinky *pinky;
Inky *inky;
Clyde *clyde;

/**
 * Fruit/Pills/General state.
 */
bool game_start = false;
uint8_t current_level = 0;
uint8_t ghost_train = 1;
bool first_fruit = false;
bool second_fruit = false;
Rect fruit_rect = Rect(0, 0, 0, 0);
Point fruit_a = Point(19, 21);
Point fruit_b = Point(20, 21);

uint32_t pill_eaten_time;
uint32_t pills_eaten;
uint32_t pills_eaten_this_life;

void theres_a_ghost_about_this_house(Timer &timer) {
  uint8_t resting_mask = (ghostState::RESTING | ghostState::LEAVING);
  bool pinky_at_home = (pinky->state & resting_mask) == ghostState::RESTING;
  bool inky_at_home = (inky->state & resting_mask) == ghostState::RESTING;
  bool clyde_at_home = (clyde->state & resting_mask) == ghostState::RESTING;

  if (!pinky_at_home && !inky_at_home && !clyde_at_home) {
    return;
  }

  uint32_t time_since_last_pill = (now() - pill_eaten_time) / 1000;
  uint32_t max_time_since_last_pill = (current_level < 5) ? 4 : 3;
  if (time_since_last_pill > max_time_since_last_pill) {
    pill_eaten_time = now();
    if (pinky_at_home) {
      return pinky->set_state(ghostState::LEAVING);
    }
    if (inky_at_home) {
      return inky->set_state(ghostState::LEAVING);
    }
    if (clyde_at_home) {
      return clyde->set_state(ghostState::LEAVING);
    }
  }

  if (lives_not_lost_this_level) {
    // Pinky leaves immediately.
    if (pinky_at_home) {
      pinky->set_state(ghostState::LEAVING);
    }
    if (inky_at_home && pills_eaten_this_life >= inky_dot_limit) {
      inky->set_state(ghostState::LEAVING);
    }
    if (clyde_at_home && pills_eaten_this_life >= clyde_dot_limit) {
      clyde->set_state(ghostState::LEAVING);
    }
    return;
  }

  // First ghost leaves at 7, then 17, then 32.
  if (pinky_at_home && pills_eaten_this_life > 6) {
    pinky->set_state(ghostState::LEAVING);
  }
  if (inky_at_home && pills_eaten_this_life > 16) {
    inky->set_state(ghostState::LEAVING);
  }
  if (clyde_at_home && pills_eaten_this_life > 31) {
    clyde->set_state(ghostState::LEAVING);
  }
}

void fruit_timer_callback(Timer &timer) { fruit_rect = Rect(0, 0, 0, 0); }

void power_timer_callback(Timer &timer) {
  ghost_train = 1;
  player->power = 0;
  Ghosts::clear_state(ghostState::FRIGHTENED);
}

void start_power_timer() {
  uint32_t ms = level_data[current_level].fright_time;

  if (power_timer.is_running()) {

    power_timer.duration += ms;
  } else {
    power_timer.duration = ms;
    power_timer.loops = 1;

    power_timer.start();
  }
}

bool operator==(Point a, Point b) { return (a.x == b.x && a.y == b.y); }

bool operator!=(Point a, Point b) { return (a.x != b.x || a.y != b.y); }

void spawn_fruit() {
  if (!first_fruit && pills_eaten >= 70) {
    first_fruit = true;
    fruit_rect = fruit_mapping[level_data[current_level].bonus_image];
    fruit_timer.start();
  } else if (first_fruit && !second_fruit && pills_eaten >= 170) {
    second_fruit = true;
    fruit_rect = fruit_mapping[level_data[current_level].bonus_image];
    fruit_timer.start();
  }
  if (fruit_rect.w != 0) {
    Point pacman_pt = get_tile(player->location);
    if (pacman_pt == fruit_a || pacman_pt == fruit_b) {
      player->score += level_data[current_level].bonus_points;
      fruit_rect = Rect(0, 0, 0, 0);
    }
  }
}

void animate_level(Timer &timer) {
  uint32_t t = now();
  player->animate();
  if (bodge_ghost_animation) {
    for (auto ghost : ghosts) {
      ghost->animate(t);
    }
  }
  bodge_ghost_animation = !bodge_ghost_animation;
}

std::array<int32_t, 8> getCycleTimes() {
  if (current_level < 1) {
    return std::array<int32_t, 8>{7000, 20000, 7000, 20000, 5000, 20000, 5000, -1};
  }
  if (current_level < 4) {
    return std::array<int32_t, 8>{7000, 20000, 7000, 20000, 5000, 1033000, 17, -1};
  }
  return std::array<int32_t, 8>{5000, 20000, 5000, 20000, 5000, 1037000, 17, -1};
}

uint8_t level_get(Point p) {
  if (p.y < 0 || p.x < 0 || p.y >= level_height || p.x >= level_width) {
    return entityType::WALL;
  }
  uint8_t entity = level_tiles[p.y * level_width + p.x];
  return mappings[entity];
}

void level_set(Point p, uint8_t e) { level_tiles[p.y * level_width + p.x] = e; }

float deg2rad(float a) { return a * (pi / 180.0f); }

uint32_t high_score;
uint32_t go_score;
uint32_t ms_update;

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
  set_screen_mode(ScreenMode::hires);
  srand(time(NULL));

  channels[0].waveforms = Waveform::TRIANGLE | Waveform::SQUARE;
  channels[0].attack_ms = 10;
  channels[0].decay_ms = 20;
  channels[0].sustain = 0x0;
  channels[0].release_ms = 15;
  channels[0].frequency = 250;

  cycleTimes = getCycleTimes();

  // Load ghost/pacman/fruit sprites
  screen.sprites = SpriteSheet::load(asset_sprites);
  // Load level sprites sheet. Maze | Pills
  level_sprites = SpriteSheet::load(asset_level);

  power_timer.init(power_timer_callback, 1000, 1);

  house_timer.init(theres_a_ghost_about_this_house, 10, -1);
  fruit_timer.init(fruit_timer_callback, 10000, 1);

  // Animate at 20fps. Though we bodge ghost animation to 10fps.
  // Note this is animation not movement.
  timer_level_animate.init(animate_level, 50, -1);

  // Setup pacman/ghosts here so timers don't crash.
  player = new Pacman();
  blinky = new Blinky();
  pinky = new Pinky();
  inky = new Inky();
  clyde = new Clyde();

  ghosts.assign({blinky, pinky, inky, clyde});

  Mat3 rotation = Mat3::identity();
  rotation *= Mat3::rotation(deg2rad(90));

  // Load level data in.
  level_tiles = (uint8_t *)malloc(level_width * level_height);
  for (auto x = 0; x < level_width * level_height; x++) {
    level_tiles[x] = asset_assets_level1_tmx[x];
  }
  level = new TileMap(level_tiles, nullptr, Size(level_width, level_height), level_sprites);
}

// Line-interrupt callback for level->draw that applies our camera
// transformation This can be expanded to add effects like camera shake, wavy
// dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 { return camera; };

void update_camera() {
  Vec2 center = Vec2(19 * 8 + 4, 18 * 8);
  camera = Mat3::identity();
  // Take a point half way between pacman and the center of the map and make it
  // the top left.
  camera *= Mat3::translation(Vec2((center.x + player->location.x) / 2, (center.y + player->location.y) / 2));
  // Transform to centre of framebuffer
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2));
  inverse_camera = Mat3(camera);
  inverse_camera.inverse();
}

int32_t clamp(int32_t v, int32_t min, int32_t max) { return v > min ? (v < max ? v : max) : min; }

// My reference point is the top_left of a nominal "center tile".
Point world_to_screen(Point point) { return Point(Vec2(point.x - 4, point.y - 4) * inverse_camera); }

/**
 * Return the tile at the center of the sprite.
 * Given sprite ref is top left of 16x16 sprite.
 */
Point get_tile(Point point) {
  Point center = Point(point.x + 3, point.y + 3);
  return Point(clamp(floor(center.x / 8), 0, level_width), clamp(floor(center.y / 8), 0, level_height));
}

void next_level() {
  lives_not_lost_this_level = true;
  first_fruit = false;
  second_fruit = false;

  // Try not to crash if you exceed the number of levels.
  if (current_level < level_data.size())
    current_level++;

  cycleTimes = getCycleTimes();
  for (auto g: ghosts)
    g->cycle_index = 0;

  for (auto x = 0; x < level_width * level_height; x++) {
    level_tiles[x] = asset_assets_level1_tmx[x];
  }
  pills_eaten = 0;

  reset_level();

  // Level 2
  if (current_level == 1) {
    inky_dot_limit = 0;
    clyde_dot_limit = 80;
  }
  // Level 3
  if (current_level >= 2) {
    inky_dot_limit = 0;
    clyde_dot_limit = 0;
  }
}

void reset_level() {
  game_start = false;

  player->init(level_data[current_level]);
  Ghosts::init(current_level);

  power_timer.stop();
  timer_level_animate.stop();
  house_timer.stop();

  ghost_train = 1;
  pills_eaten_this_life = 0;
}

void game_over() {
  current_level = 0;
  pills_eaten = 0;
  // Reset cycle times.
  cycleTimes = getCycleTimes();
  // Restore pills.
  for (auto x = 0; x < level_width * level_height; x++) {
    level_tiles[x] = asset_assets_level1_tmx[x];
  }
  for (auto g: ghosts) {
    // Reset ghost move cycle.
    g->cycle_index = 0;
  }
  player->lives = 4;
  player->score = 0;
  reset_level();
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t t) {
  // clear the screen -- screen is a reference to the frame buffer and can be
  // used to draw all things with the 32blit
  screen.pen = Pen(0, 0, 0);
  screen.clear();

  uint32_t ms_start = now();
  // Draw maze and pills.
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);

  if (fruit_rect.w > 0) {
    screen.sprite(fruit_rect, world_to_screen(Point(156, 168)));
  }

  player->render();
  for (auto ghost : ghosts) {
    ghost->render();
  }
  uint32_t ms_end = now();

  // Draw the header bar
  screen.pen = Pen(0, 0, 1);
  screen.rectangle(Rect(0, 0, screen_width, 10));
  screen.pen = Pen(255, 255, 255);
  screen.text("Score " + std::to_string(player->score), minimal_font, Point(30, 2));

  // Draw game over screen.
  if (player->lives == 0) {
    screen.pen = Pen(0, 0, 1);
    screen.rectangle(Rect(0, (screen_height / 2) - 20, screen_width, 40));
    screen.pen = Pen(255, 255, 0);
    screen.text(" GAME OVER ", fat_font, Point(screen_width / 2 - 20, screen_height / 2 - 15));
    screen.text(" Your score " + std::to_string(go_score), minimal_font,
                Point(screen_width / 2 - 20, screen_height / 2 + 10));
  } else if (!game_start) {
    screen.pen = Pen(255, 255, 0);
    screen.text("READY!", fat_font, Point(150, 108));
  }

  // Debug ghost target.
  screen.pen = Pen(255,0,0);
  Point blinky_target = blinky->get_target() * 8;
  // Offset debug of blinky so we can see clydes move.
  screen.circle(world_to_screen(Point(blinky_target.x + 2, blinky_target.y)), 2);
  screen.pen = Pen(255,128,128);
  Point pinky_target = pinky->get_target() * 8;
  screen.circle(world_to_screen(pinky_target), 2);
  screen.pen = Pen(0,0,255);
  Point inky_target = inky->get_target() * 8;
  screen.circle(world_to_screen(inky_target), 2);
  screen.pen = Pen(200,200,128);
  Point clyde_target = clyde->get_target() * 8;
  screen.circle(world_to_screen(clyde_target), 2);
  

  // Draw footer bar
  screen.pen = Pen(0, 0, 1);
  screen.rectangle(Rect(0, screen_height - 20, screen_width, 20));
  player->render_lives();

  screen.pen = Pen(20, 255, 20);
  std::string fms = "U: " + std::to_string(ms_update) + " R: " + std::to_string(ms_end - ms_start);
  screen.text(fms, minimal_font, Point(screen_width - 60, screen_height - 16));

  screen.pen = Pen(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
  last_time = time;

  static uint32_t last_animation = time;
  static uint32_t button_debounce = time;
  if (game_start && player->lives > 0) {
    player->update(time);
    bool pacman_eaten = Ghosts::update(time);
    if (pacman_eaten) {
      lives_not_lost_this_level = false;
      if (--player->lives == 0) {
        go_score = player->score;
        high_score = (player->score > high_score) ? player->score : high_score;
      }
      reset_level();
    } else if (pills_eaten == pills_per_level) {
      next_level();
    }
    spawn_fruit();
    // Debounce pause.
  } else if (buttons & Button::A && time - button_debounce > button_debounce_rate) {
    button_debounce = time;
    if (player->lives == 0) {
      game_over();
    }
    if (!game_start) {
      game_start = true;
      timer_level_animate.start();
      house_timer.start();
      Ghosts::move_start();
    }
  }
  update_camera();

  ms_update = now() - time;
}
