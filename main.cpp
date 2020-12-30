#include <time.h>
#include "main.hpp"
#include "assets.hpp"
#include "pacman.hpp"
#include "ghosts.hpp"
#include "ghost.hpp"

using namespace blit;

Mat3 camera;

uint8_t *level_data;
uint8_t *pill_data;

TileMap *level;

Map map(blit::Rect(0, 0, level_height, level_height));

bool bodge_ghost_animation = false;

Timer timer_level_animate;
Timer power_timer;

Pacman *player;
Blinky *blinky;
Pinky *pinky;
Inky *inky;
Clyde *clyde;

uint32_t pills_eaten;
uint32_t pills_eaten_this_life;

std::vector<Ghost *> ghosts;

void power_timer_callback(Timer &timer) {
  printf("Power pill expired.\n");
  player->power = 0;
  for (auto ghost: ghosts) {
    ghost->clear_state(ghostState::FRIGHTENED);
  }
}

void start_power_timer(uint32_t ms) {
  if (power_timer.is_running()) {
    printf("Increasing power timer.\n");
    power_timer.duration += ms;
  } else {
    printf("Starting power timer.\n");
    power_timer.duration = ms;
    power_timer.loops = 1;
    power_timer.start();
  }
}

void set_ghost_state(ghostState s) {
  for (auto ghost : ghosts) {
    ghost->set_state(s);
  }
}

bool operator==(Point a, Point b) {
  return (a.x == b.x && a.y == b.y);
}

bool operator!=(Point a, Point b) {
  return (a.x != b.x || a.y != b.y);
}

void animate_level(Timer &timer) {
  player->animate();
  if (bodge_ghost_animation) {
    for (auto ghost : ghosts) {
      ghost->animate();
    }
  }
  bodge_ghost_animation = !bodge_ghost_animation;
}

entityType level_get(Point p) {
  if(p.y < 0 || p.x < 0 || p.y >= level_height || p.x >= level_width) {
    return entityType::WALL;
  }
  entityType entity = (entityType)pill_data[p.y * level_width + p.x];
  return entity;
}

void level_set(Point p, entityType e) {
  pill_data[p.y * level_width + p.x] = e;
}

float deg2rad(float a) {
    return a * (pi / 180.0f);
}

bool game_start;
bool game_paused;
uint32_t high_score;
uint32_t go_score;

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
  srand(time(NULL));
  game_start = false;
  game_paused = true;
  set_screen_mode(ScreenMode::hires);

  power_timer.init(power_timer_callback, 1000, 1);
  
  timer_level_animate.init(animate_level, 50, -1);

  // Setup pacman/ghosts here so timers don't crash.
  player = new Pacman();
  blinky = new Blinky();
  pinky = new Pinky();
  inky = new Inky();
  clyde = new Clyde();

  ghosts.assign({blinky, pinky, inky, clyde});

  // Load sprite sheet.
  screen.sprites = SpriteSheet::load(asset_level);

  // Malloc memory for level.
  level_data = (uint8_t *)malloc(level_width * level_height);
  
  Mat3 rotation = Mat3::identity();
  rotation *= Mat3::rotation(deg2rad(90));

  // Load level data in.
  level = new TileMap((uint8_t *)level_data, nullptr, Size(level_width, level_height), screen.sprites);
  for(auto x = 0; x < level_width * level_height; x++){
    level_data[x] = asset_assets_level1_tmx[x];
  }
  std::vector<uint8_t> mazeVector(asset_assets_level1_tmx_length);
  mazeVector.assign(&asset_assets_level1_tmx[0], &asset_assets_level1_tmx[asset_assets_level1_tmx_length]);

  map.add_layer("background", mazeVector);
  // Set walls.
  map.layers["background"].add_flags({
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    16,17,22,23,24,25,26,27,28,29,30,31,
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,58,59,60,61
  }, entityType::WALL);
  map.layers["background"].add_flags(62, entityType::JUNCTION);
  map.layers["background"].add_flags(63, entityType::WARP);
  map.layers["background"].add_flags(254, entityType::PORTAL);
  map.layers["background"].add_flags({0,62,63}, entityType::NOTHING);
  // Set pills.
  pill_data = (uint8_t *)malloc(level_width * level_height);
  for(auto x = 0; x < level_width * level_height; x++){
    pill_data[x] = asset_pills[x];
  }
}

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

void update_camera() {
  camera = Mat3::identity();
  camera *= Mat3::translation(Vec2(player->location.x, player->location.y)); // offset to middle of world
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
}

int32_t clamp(int32_t v, int32_t min, int32_t max) {
  return v > min ? (v < max ? v : max) : min;
}

/** 
 * Was called feet 
 * Don't understand why we neet to futz with width height.
 * If my sprite is 156x 120y 16:16 why does tiles_in_rect
 * check 3 deep.  I can understand it checking 3 wide as 156+16 covers three tiles.
 * But 120+16 does not! FFS FFS Swear rant.
 **/
Rect footprint(Point pos, Size size) {
  // return Rect(pos.x, pos.y, size.w, size.h / 2);
  return Rect(pos,size);
}

Point world_to_screen(Point point) {
  Vec2 world_vector = Vec2(point.x, point.y);
  Mat3 bob = Mat3(camera);
  bob.inverse();
  Vec2 screen_vector = world_vector * bob;
  return Point(screen_vector);
}

Point screen_to_world(Point point) {
  Vec2 screen_vector = Vec2(point.x,point.y);
  Vec2 world_vector = screen_vector * camera;
  return Point(world_vector);
}

Point tile(Point point) {
  return Point(clamp(point.x/8,0,level_width),clamp(point.y/8,0,level_height));
}

void draw_layer(uint8_t *layer) {
  draw_layer(layer, 0);
}

void draw_layer(uint8_t *layer, int32_t offset) {
  Point tl = screen_to_world(Point(0, 0));
  Point br = screen_to_world(Point(screen.bounds.w, screen.bounds.h));

  Point tlt = tile(tl);
  Point brt = tile(br);

  for (uint8_t y = tlt.y; y <= brt.y; y++) {
    for (uint8_t x = tlt.x; x <= brt.x; x++) {
      Point pt = world_to_screen(Point(x * 8 + offset, y * 8 + offset));
      int32_t ti = y * level_width + x;
      if (ti != -1) {
        uint8_t si = pill_data[ti];
        if (si != 0) {
          screen.sprite(si, pt);
        }
      }
    }
  }
}

void next_level() {
  for(auto x = 0; x < level_width * level_height; x++){
    pill_data[x] = asset_pills[x];
  }
  pills_eaten = 0;
  reset_level();
}

void pause_game() {
  printf("main::pausing game.\n");
  for (auto ghost : ghosts) {
    ghost->move_cycle_timer.pause();
  }
  power_timer.pause();
  game_paused = true;
}

void resume_game() {
  printf("main::resume_game resuming game %d.\n", power_timer.state);
  for (auto ghost : ghosts) {
    // Don't resume move cycle timer unless it is paused.
    if ((ghost->state & ghostState::FRIGHTENED) == 0) {
      printf("resume_game %s move resume.\n", ghost->name.c_str());
      ghost->move_cycle_timer.start();
    }
  }
  if (power_timer.state & Timer::PAUSED) {
    printf("main::resume_game resuming power pill.\n");
    power_timer.start();
  }
  game_paused = false;
  if (!timer_level_animate.is_running()) {
    timer_level_animate.start();
  }
}

void reset_level() {
  player->init();
  blinky->init();
  pinky->init();
  inky->init();
  clyde->init();
  power_timer.stop();
  timer_level_animate.stop();

  blinky_cycle_index = 0;
  pinky_cycle_index = 0;
  inky_cycle_index = 0;
  clyde_cycle_index = 0;

  pills_eaten_this_life = 0;
  game_paused = true;
}

void game_over() {
  game_start = false;
  game_paused = true;
  pills_eaten = 0;
  pills_eaten_this_life = 0;
  // Restore pills.
  for(auto x = 0; x < level_width * level_height; x++){
    pill_data[x] = asset_pills[x];
  }
  player->new_game();
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t t) {
  // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
  screen.pen = Pen(0, 0, 0);
  screen.clear();
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);
  
  draw_layer(pill_data, 4);

  player->render();

  for (auto ghost : ghosts) {
    ghost->render();
  }

  // Draw the header bar
  screen.pen = Pen(0, 0, 1);
  screen.rectangle(Rect(0, 0, screen_width, 10));
  screen.pen = Pen(255, 255, 255);
  screen.text("    " + std::to_string(player->score), minimal_font, Point(2, 2));

  // Draw game over screen.
  if (player->lives == 0) {
    screen.pen = Pen(0, 0, 1);
    screen.rectangle(Rect(0, (screen_height/2) - 20, screen_width, 40));
    screen.pen = Pen(255, 255, 255);
    screen.text(" GAME OVER ", fat_font, Point(screen_width/2 - 20, screen_height/2 - 15));
    screen.text(" Your score " + std::to_string(go_score), minimal_font, Point(screen_width/2 - 20, screen_height/2 + 10));
  }
  
  screen.pen = Pen(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t t) {
  static uint32_t last_animation = t;
  static uint32_t button_debounce = t;
  if (game_start && !game_paused && player->lives > 0) {
    player->update(t);

    if (inky->state & ghostState::RESTING && pills_eaten_this_life >= 30) {
      inky->set_state(ghostState::LEAVING);
    }

    if (clyde->state & ghostState::RESTING && pills_eaten_this_life >= 60) {
      clyde->set_state(ghostState::LEAVING);
    }

    Point pacman_pt = tile(player->location);
    bool capman = false;
    for (auto ghost : ghosts) {
      ghost->update(t);
      Point ghost_pt = tile(ghost->location);
      if (pacman_pt == ghost_pt) {
        if (ghost->edible()) {
          ghost->set_state(ghostState::EATEN);
          player->score += 100;
        } else if (!ghost->eaten()) {
          printf("%s ate pacman.\n", ghost->name.c_str());
          capman = true;
          for (auto ghost : ghosts) {
            ghost->move_cycle_timer.pause();
          }
        }
      }
    }

    if (capman) {
      if (--player->lives == 0) {
        go_score = player->score;
        high_score = (player->score > high_score) ? player->score : high_score;
      }
      reset_level();
    } else if (pills_eaten == pills_per_level) {
      next_level();
    }

    // Debounce pause.
    if (buttons & Button::A && t - button_debounce > button_debounce_rate) {
      button_debounce = t;
      pause_game();
    }
  // Debounce pause.
  } else if (buttons & Button::A && t - button_debounce > button_debounce_rate) {
    button_debounce = t;
    if (player->lives == 0) {
      game_over();
    }
    if (!game_start) {
      printf("main::update starting game.\n");
      game_start = true;
    } 
    resume_game();
  }
  update_camera();
}