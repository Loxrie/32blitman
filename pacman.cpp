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

const bool debug_logging = false;

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
  Vec2 location;
  Vec2 movement;
  uint32_t direction;

  Vec2 desired_movement;
  uint32_t desired_direction;

  entityType moving_to;

  // std::vector<Point> debug_bounds = {Point(0,0), Point(0,0), Point(0,0), Point(0,0)};
  
  uint8_t sprite;
  bool animation_direction;

  uint32_t score;
  
  uint32_t level;
  uint32_t lives;

  Player() {
    sprite = 0;
    lives = 3;
    score = 0;
    location = Vec2((19*8)+4,21*8);
    movement = Vec2(0,0);
    direction = 0;
    desired_movement = movement;
    desired_direction = direction;
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
    return feet(location);
  }

  Rect feet(Vec2 location) {
    return Rect(location.x, location.y, size.w - 1, size.h - 1);
  }

  std::function<void(Point)> collision_detection = [this](Point tile_pt) -> void {
    if(this->moving_to == entityType::NOTHING && map.has_flag(tile_pt, entityType::WALL)) {
      this->moving_to = entityType::WALL;
    }
  };

  void update(uint32_t time) {
    Vec2 t_location = Vec2(location.x,location.y);
    moving_to = entityType::NOTHING;
    Rect bounds_lr;

    if (buttons.state > 0) {
      // Possible new direction.
      if(buttons & Button::DPAD_UP) {
        desired_movement = Vec2(0,-1);
        desired_direction = Button::DPAD_UP;
      } 
      else if(buttons & Button::DPAD_DOWN) {
        desired_movement = Vec2(0,1);
        desired_direction = Button::DPAD_DOWN;
      }
      else if(buttons & Button::DPAD_LEFT) {
        desired_movement = Vec2(-1,0);
        desired_direction = Button::DPAD_LEFT;
      }
      else if(buttons & Button::DPAD_RIGHT) {
        desired_movement = Vec2(1,0);
        desired_direction = Button::DPAD_RIGHT;
      }
    }

    // See if new input direction is valid.
    t_location += desired_movement;
    bounds_lr = feet(t_location);
    map.tiles_in_rect(bounds_lr, collision_detection);

    // Try existing direction.
    if (moving_to == entityType::WALL) {
      // Continue on.
      moving_to = entityType::NOTHING;
      t_location = location + movement;
      bounds_lr = feet(t_location);
      map.tiles_in_rect(bounds_lr, collision_detection);
    } else {
      movement = desired_movement;
      direction = desired_direction;
    }

    if (moving_to == entityType::NOTHING) {
      location = t_location;
    }

    // this->debug_bounds = { 
    //   Point(bounds_lr.x, bounds_lr.y), 
    //   Point(bounds_lr.x + bounds_lr.w, bounds_lr.y), 
    //   Point(bounds_lr.x + bounds_lr.w, bounds_lr.y + bounds_lr.h),
    //   Point(bounds_lr.x, bounds_lr.y + bounds_lr.h)
    // };
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
  // Set walls.
  map.layers["background"].add_flags({
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    16,17,22,23,24,25,26,27,28,29,30,31,
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
  }, entityType::WALL);
  // Set pills.
  std::vector<uint8_t> pillVector(asset_pills_length);
  pillVector.assign(&asset_pills[0], &asset_pills[asset_pills_length]);
  map.add_layer("pills", pillVector);
  map.layers["pills"].add_flags(240, entityType::PILL);
}

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

void update_camera() {
  camera = Mat3::identity();
  camera *= Mat3::translation(Vec2(player.location.x, player.location.y)); // offset to middle of world
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
}

Point world_to_screen(Point point) {
  Vec2 world_vector = Vec2(point.x, point.y);
  Mat3 bob = Mat3(camera);
  bob.inverse();
  Vec2 screen_vector = world_vector * bob;
  return Point(screen_vector.x, screen_vector.y);
}

Point screen_to_world(Point point) {
  Vec2 screen_vector = Vec2(point.x,point.y);
  Vec2 world_vector = screen_vector * camera;
  return Point(world_vector.x,world_vector.y);
}

Point tile(Point point) {
  return point/8;
}

void draw_layer(MapLayer &layer, uint32_t offset) {
  Point tl = screen_to_world(Point(0, 0));
  Point br = screen_to_world(Point(screen.bounds.w, screen.bounds.h));

  Point tlt = tile(tl);
  Point brt = tile(br);

  for (uint8_t y = tlt.y; y <= brt.y; y++) {
    for (uint8_t x = tlt.x; x <= brt.x; x++) {
      Point pt = world_to_screen(Point(x * 8 + offset, y * 8 + offset));
      int32_t ti = layer.map->tile_index(Point(x, y));
      if (ti != -1) {
        uint8_t si = layer.tiles[ti];
        if (si != 0) {
          screen.sprite(si, pt);
        }
      }
    }
  }
}

void draw_layer(MapLayer &layer) {
  draw_layer(layer, 0);
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
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);
  
  draw_layer(map.layers["pills"], 4);

  screen.sprite(pacmanAnims[player.sprite], world_to_screen(player.location));

  // if (debug_logging) {
  //   screen.pen = Pen(255,0,255);
  //   screen.pixel(player.location);
  //   Point prev_point = Point(-1,-1);
  //   for(Point a_point: player.debug_bounds) {
  //     if (prev_point.x != -1) {
  //       screen.line(prev_point, a_point);
  //       prev_point = a_point;
  //     } else {
  //       prev_point = a_point;
  //     }
  //   }
  //   screen.line(player.debug_bounds[0], player.debug_bounds[player.debug_bounds.size()-1]);
  // }

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
  static uint32_t last_animation = time;
  player.update(time);
  if (time - last_animation > 1000/30) {
    last_animation = time;
    player.anim_player();
  }
  update_camera();
}
