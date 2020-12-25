#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "assets.hpp"

using namespace blit;

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

enum entityType {
  NOTHING = 0x00,
  WALL = 0x01,
  PILL = 0x02,
  POWER = 0x03,
  GHOST = 0x04,
};

const bool debug_logging = true;

Rect pacmanAnims[16] = {
  // Left
  Rect(12,14,2,2),
  Rect(4,12,2,2),
  Rect(2,12,2,2),
  Rect(0,12,2,2),
  // Right
  Rect(12,14,2,2),
  Rect(10,12,2,2),
  Rect(8,12,2,2),
  Rect(6,12,2,2),
  // Up
  Rect(12,14,2,2),
  Rect(4,14,2,2),
  Rect(2,14,2,2),
  Rect(0,14,2,2),
  // Down
  Rect(12,14,2,2),
  Rect(10,14,2,2),
  Rect(8,14,2,2),
  Rect(6,14,2,2),
};

Mat3 camera;

uint8_t *level_data;

Map map(Rect(0, 0, level_height, level_height));
TileMap *level;

struct Player {
  Size size = Size(16, 16);
  Point screen_location;
  Point movement;
  uint32_t direction;
  entityType moving_to;

  std::vector<Point> debug_bounds = {Point(0,0), Point(0,0), Point(0,0), Point(0,0)};
  
  uint8_t sprite;
  bool animation_direction;

  uint32_t score;
  
  uint32_t level;
  uint32_t lives;

  bool dead;

  Player() {
    sprite = 0;
    dead = false;
    lives = 3;
    score = 0;
    screen_location = Point((19*8)+4,21*8);
    movement = Point(0,0);
    direction = 0;
  }

  void anim_player() {
    uint8_t offset = 0;
    switch(direction) {
      case Button::DPAD_LEFT:
        offset = 0;
        break;
      case Button::DPAD_RIGHT:
        offset = 4;
        break;
      case Button::DPAD_UP:
        offset = 8;
        break;
      case Button::DPAD_DOWN:
        offset = 12;
        break;
    }
    bool animDirection = animation_direction;
    switch (sprite % 4) {
      case 0: 
        sprite = offset + 1;
        animation_direction = true;
        break;
      case 1:
        sprite = (animDirection) ? offset + 2 : offset + 0;
        break;
      case 2:
        sprite = (animDirection) ? offset + 3 : offset + 1;
        break;
      case 3:
        sprite = offset + 2;
        animation_direction = false;
        break;
    }
    if (sprite > 15) {
      printf("Warning sprite %d out of range.", sprite);
      sprite = 0;
    }
  }

  Rect feet() {
    return Rect(screen_location.x, screen_location.y, size.w - 1, size.h - 1);
  }

  std::function<void(Point)> collision_detection = [this](Point tile_pt) -> void {
    if(this->moving_to == entityType::NOTHING && map.has_flag(tile_pt, entityType::WALL)) {
      this->moving_to = entityType::WALL;
    }
  };

  void update(uint32_t time) {
    static uint32_t last_pacman_animation = time;
    
    Point new_movement = Point(0,0);
    uint32_t new_direction = 0;
    
    Point restoreScreenLocation = screen_location;
    moving_to = entityType::NOTHING;
    Rect bounds_lr;

    if (buttons.state > 0) {
      // Possible new direction.
      if(buttons & Button::DPAD_UP) {
        new_movement = Point(0,-1);
        new_direction = Button::DPAD_UP;
      } 
      else if(buttons & Button::DPAD_DOWN) {
        new_movement = Point(0,1);
        new_direction = Button::DPAD_DOWN;
      }
      else if(buttons & Button::DPAD_LEFT) {
        new_movement = Point(-1,0);
        new_direction = Button::DPAD_LEFT;
      }
      else if(buttons & Button::DPAD_RIGHT) {
        new_movement = Point(1,0);
        new_direction = Button::DPAD_RIGHT;
      }
      screen_location += new_movement;
      bounds_lr = feet();
      map.tiles_in_rect(bounds_lr, collision_detection);

      // We hit a wall try existing movement.
      if (moving_to == entityType::WALL) {
        // Continue on.
        moving_to = entityType::NOTHING;
        new_movement = movement;
        new_direction = direction;
        screen_location = restoreScreenLocation + new_movement;
      }
    } else {
      // Continue on.
      new_movement = movement;
      new_direction = direction;
      screen_location += new_movement;
    }
      
    // If moving to is NOTHING check collision.
    if (moving_to == entityType::NOTHING) {
      bounds_lr = feet();
      map.tiles_in_rect(bounds_lr, collision_detection);
    }
    
    this->debug_bounds = { 
      Point(bounds_lr.x, bounds_lr.y), 
      Point(bounds_lr.x + bounds_lr.w, bounds_lr.y), 
      Point(bounds_lr.x + bounds_lr.w, bounds_lr.y + bounds_lr.h),
      Point(bounds_lr.x, bounds_lr.y + bounds_lr.h)
    };

    if (moving_to == entityType::NOTHING) {
      // Animate at 60fps.
      if (time - last_pacman_animation > (1000/60)) {
        last_pacman_animation = time;
        movement = new_movement;
        direction = new_direction;
        anim_player();
      }
    } else {
      screen_location = restoreScreenLocation;
      movement = Point(0,0);
    }
  }
} player;

float deg2rad(float a) {
    return a * (pi / 180.0f);
}

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
  set_screen_mode(ScreenMode::hires);

  // Load sprite sheet.
  screen.sprites = SpriteSheet::load(asset_sprites);

  // Malloc memory for level.
  level_data = (uint8_t *)malloc(level_width * level_height);
  
  Mat3 rotation = Mat3::identity();
  rotation *= Mat3::rotation(deg2rad(90));

  // Load level data in.
  level = new TileMap((uint8_t *)level_data, nullptr, Size(level_width, level_height), screen.sprites);
  for(auto x = 0; x < level_width * level_height; x++){
    level_data[x] = asset_assets_level2_tmx[x];
  }
  std::vector<uint8_t> mazeVector(asset_assets_level2_tmx_length);
  mazeVector.assign(&asset_assets_level2_tmx[0], &asset_assets_level2_tmx[asset_assets_level2_tmx_length]);
  // printf("Length of level and vector %d / %lu\n", asset_assets_level2_tmx_length, mazeVector.size());

  map.add_layer("background", mazeVector);
  map.layers["background"].add_flags({
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    16,17,22,23,24,25,26,27,28,29,30,31,
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
  }, entityType::WALL);

  
}

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

entityType level_get(Point n_location) {
  // Location is top left of pacman.
  if(n_location.y < 0 || n_location.x < 0 || n_location.y >= level_height || n_location.x >= level_width) {
    return entityType::WALL;
  }
  entityType entity = entityType::NOTHING;
  
  uint8_t tl_wall = map.has_flag(n_location, entityType::WALL);
  uint8_t tr_wall = map.has_flag(Point(n_location.x, n_location.y + 1), entityType::WALL);
  uint8_t bl_wall = map.has_flag(Point(n_location.x + 1, n_location.y), entityType::WALL);
  uint8_t br_wall = map.has_flag(Point(n_location.x + 1, n_location.y + 1), entityType::WALL);
  if (tl_wall || tr_wall || bl_wall || br_wall) {
    entity = entityType::WALL;
  }
  // printf("Player %d:%d moving into tile %d = %d\n", n_location.x,n_location.y, map.tile_index(n_location), entity);
  return entity;
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {
  // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
  screen.pen = Pen(0, 0, 0);
  screen.clear();
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), nullptr);
  // screen.sprite(Point(5,2), Point(32,32));

  if(!player.dead) {
    screen.sprite(pacmanAnims[player.sprite], player.screen_location);
  }

  if (debug_logging) {
    screen.pen = Pen(255,0,255);
    screen.pixel(player.screen_location);
    Point prev_point = Point(-1,-1);
    for(Point a_point: player.debug_bounds) {
      if (prev_point.x != -1) {
        screen.line(prev_point, a_point);
        prev_point = a_point;
      } else {
        prev_point = a_point;
      }
    }
    screen.line(player.debug_bounds[0], player.debug_bounds[player.debug_bounds.size()-1]);
    // screen.polygon(player.debug_bounds);
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
void update(uint32_t time) {
  player.update(time);
}
