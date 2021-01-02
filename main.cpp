#include <time.h>
#include "main.hpp"
#include "assets.hpp"
#include "pacman.hpp"
#include "ghosts.hpp"
#include "ghost.hpp"

using namespace blit;

Mat3 camera;

// uint8_t *pill_data;
uint8_t *level_tiles;
uint8_t current_level;

TileMap *level;
// TileMap *pills;

SpriteSheet *level_sprites;

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

void power_timer_callback(Timer &timer) {

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

bool operator==(Point a, Point b) {
  return (a.x == b.x && a.y == b.y);
}

bool operator!=(Point a, Point b) {
  return (a.x != b.x || a.y != b.y);
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

uint8_t level_get(Point p) {
  if(p.y < 0 || p.x < 0 || p.y >= level_height || p.x >= level_width) {
    return entityType::WALL;
  }
  uint8_t entity = level_tiles[p.y * level_width + p.x];
  return mappings[entity];
}

void level_set(Point p, uint8_t e) {
  level_tiles[p.y * level_width + p.x] = e;
}

float deg2rad(float a) {
    return a * (pi / 180.0f);
}

bool game_start;
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
  current_level = 0;
  srand(time(NULL));
  game_start = false;
  set_screen_mode(ScreenMode::hires);

  // Load level sprites sheet. Maze | Pills | Fruit
  screen.sprites = SpriteSheet::load(asset_sprites);
  // Load ghost/pacman sprites
  level_sprites = SpriteSheet::load(asset_leveltng);
  
  power_timer.init(power_timer_callback, 1000, 1);
  
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
  for(auto x = 0; x < level_width * level_height; x++){
    level_tiles[x] = asset_assets_leveltng_tmx[x];
  }
  level = new TileMap(level_tiles, nullptr, Size(level_width, level_height), level_sprites);
  std::vector<uint8_t> mazeVector(asset_assets_leveltng_tmx_length);
  mazeVector.assign(&asset_assets_leveltng_tmx[0], &asset_assets_leveltng_tmx[asset_assets_level1_tmx_length]);

  map.add_layer("background", mazeVector);
  // Set walls.
  map.layers["background"].add_flags({
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
    33,34,35,36,37,38,39,40,41,42
  }, entityType::WALL);
  map.layers["background"].add_flags({ 44, 45 }, entityType::JUNCTION);
  map.layers["background"].add_flags(42, entityType::TUNNEL);
  map.layers["background"].add_flags(43, entityType::PORTAL);
}

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

// Nasty hack to offset pills from tiles to appear at the center of the maze paths.
std::function<Mat3(uint8_t)> pill_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera * Mat3::translation(Vec2(-4, -4));
};

void update_camera() {
  Vec2 center = Vec2(19*8+4,18*8);
  camera = Mat3::identity();
  // camera *= Mat3::translation(Vec2(player->location.x, player->location.y)); // offset to middle of world
  camera *= Mat3::translation(Vec2((center.x+player->location.x)/2, (center.y+player->location.y)/2)); // Move middle of world - pacman to top left.
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
}

int32_t clamp(int32_t v, int32_t min, int32_t max) {
  return v > min ? (v < max ? v : max) : min;
}

// My reference point is the top_left of a nominal "center tile".
Point world_to_screen(Point point) {
  Vec2 world_vector = Vec2(point.x - 4, point.y - 4);
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

/**
 * Return the tile at the center of the sprite.
 * Given sprite ref is top left of 16x16 sprite.
 */ 
Point tile(Point point) {
  return Point(clamp((point.x + 7)/8,0,level_width),clamp((point.y + 7)/8,0,level_height));
}

void next_level() {

  current_level++;

  for(auto x = 0; x < level_width * level_height; x++){
    level_tiles[x] = asset_assets_leveltng_tmx[x];
  }
  pills_eaten = 0;

  blinky_cycle_index = 0;
  pinky_cycle_index = 0;
  inky_cycle_index = 0;
  clyde_cycle_index = 0;

  Ghosts::move_reset_and_pause();

  reset_level();
}

void pause_game() {

  Ghosts::move_pause();
  if (power_timer.is_running()) {

    power_timer.pause();
  }
}

void resume_game() {

  Ghosts::move_resume();
  if (power_timer.is_paused()) {

    power_timer.start();
  }
  if (!timer_level_animate.is_running()) {
    timer_level_animate.start();
  }
}

void reset_level() {
  game_start = false;

  player->init(level_data[current_level]);
  Ghosts::move_pause();
  Ghosts::init(current_level);

  power_timer.stop();
  timer_level_animate.stop();

  pills_eaten_this_life = 0;
}

void game_over() {
  game_start = false;
  pills_eaten = 0;
  pills_eaten_this_life = 0;
  // Restore pills.
  for(auto x = 0; x < level_width * level_height; x++){
    level_tiles[x] = asset_assets_leveltng_tmx[x];
  }
  player->lives = 4;
  player->score = 0;
  player->init(level_data[0]);
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

  uint32_t ms_start = now();
  // Draw maze.
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);

  // LOL i swear this is faster than drawing the layer before.
  // pills->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), pill_line_interrupt_callback);
  
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
    screen.rectangle(Rect(0, (screen_height/2) - 20, screen_width, 40));
    screen.pen = Pen(255, 255, 255);
    screen.text(" GAME OVER ", fat_font, Point(screen_width/2 - 20, screen_height/2 - 15));
    screen.text(" Your score " + std::to_string(go_score), minimal_font, Point(screen_width/2 - 20, screen_height/2 + 10));
  }
  
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
void update(uint32_t t) {
  if (power_timer.is_running()) {

  }
  static uint32_t last_animation = t;
  static uint32_t button_debounce = t;
  if (game_start && player->lives > 0) {
    player->update(t);

    // DOH SHOUTY FIX, move out to timer?
    // We could move a lot of generic game state to a timer, this, house management etc.?
    if (inky->state & ghostState::RESTING && (inky->state & ghostState::LEAVING) == 0 && pills_eaten_this_life >= 30) {
      inky->set_state(ghostState::LEAVING);
    }

    if (clyde->state & ghostState::RESTING && (clyde->state & ghostState::LEAVING) == 0 && pills_eaten_this_life >= 60) {
      clyde->set_state(ghostState::LEAVING);
    }

    bool pacman_eaten = Ghosts::update(t);
    if (pacman_eaten) {
      if (--player->lives == 0) {
        go_score = player->score;
        high_score = (player->score > high_score) ? player->score : high_score;
      }
      reset_level();
    } else if (pills_eaten == pills_per_level) {
      next_level();
    }
  // Debounce pause.
  } else if (buttons & Button::A && t - button_debounce > button_debounce_rate) {
    button_debounce = t;
    if (player->lives == 0) {
      game_over();
    }
    if (!game_start) {

      game_start = true;
      timer_level_animate.start();
      Ghosts::move_start();
    }
  }
  update_camera();

  ms_update = now() - t;
}