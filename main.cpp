#include "main.hpp"
#include "assets.hpp"
#include "pacman.hpp"
#include "ghost.hpp"

using namespace blit;

Mat3 camera;

uint8_t *level_data;

TileMap *level;

Map map(blit::Rect(0, 0, level_height, level_height));

Pacman player;
Ghost ghost;

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

Rect ghostAnims[8] = {
  // Left
  Rect(0,4,2,2),
  Rect(2,4,2,2),
  // Right
  Rect(4,14,2,2),
  Rect(6,12,2,2),
  // Up
  Rect(8,14,2,2),
  Rect(10,14,2,2),
  // Down
  Rect(12,14,2,2),
  Rect(14,14,2,2)
};

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
  map.layers["pills"].add_flags(241, entityType::POWER);
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

int32_t clamp(int32_t v, int32_t min, int32_t max) {
  return v > min ? (v < max ? v : max) : min;
}

Rect feet(Vec2 location, Size size) {
  return Rect(location.x, location.y, size.w - 1, size.h - 1);
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

void draw_layer(MapLayer &layer) {
  draw_layer(layer, 0);
}

void draw_layer(MapLayer &layer, int32_t offset) {
  Point tl = screen_to_world(Point(0, 0));
  Point br = screen_to_world(Point(screen.bounds.w, screen.bounds.h));

  Point tlt = tile(tl);
  Point brt = tile(br);

  // printf("TL Tile %d:%d\tBR Tile %d:%d\n",tlt.x,tlt.y,brt.x,brt.y);
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

  screen.sprite(ghostAnims[ghost.sprite], world_to_screen(ghost.location));

  screen.pen = Pen(255,0,255);
  screen.line(world_to_screen(ghost.location), world_to_screen(ghost.target));
  
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
  ghost.update(time);
  if (time - last_animation > 1000/30) {
    last_animation = time;
    player.anim_player();
  }
  update_camera();
}