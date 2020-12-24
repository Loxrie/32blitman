#include <cstdlib>
#include <iostream>

#include "pacman.hpp"
#include "assets.hpp"

using namespace blit;

constexpr uint16_t screen_width = 320;
constexpr uint16_t screen_height = 240;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

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

struct Player {
  Point start;
  Point position;
  Point screen_location;
  uint8_t sprite;
  uint32_t score;
  Vec2 camera;
  Point size = Point(1, 1);
  bool facing = true;
  bool has_key;
  uint32_t level;
  uint32_t lives;
  bool dead;
};

Player player;

Mat3 camera;

uint8_t *level_data;

TileMap *level;

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

  player.start = Point(19*8+4,21*8);
  player.position = player.start;
  player.camera = Vec2(player.position.x, player.position.y);
  player.sprite = 0;
  player.dead = false;
  player.lives = 3;
  player.score = 0;
  player.screen_location = Point(19*8+4,21*8);
}

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

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
  screen.pen = Pen(0, 0, 0);
  // screen.sprite(Point(5,2), Point(32,32));

  if(!player.dead) {
    screen.sprite(pacmanAnims[player.sprite], player.screen_location);
  }
}

void update_camera(uint32_t time) {
  static uint32_t thunk_a_bunch = 0;
  // Create a camera transform that centers around the player's position
  if(player.camera.x < player.position.x) {
    player.camera.x += 0.1;
  }
  if(player.camera.x > player.position.x) {
    player.camera.x -= 0.1;
  }
  if(player.camera.y < player.position.y) {
    player.camera.y += 0.1;
  }
  if(player.camera.y > player.position.y) {
    player.camera.y -= 0.1;
  }

  camera = Mat3::identity();
  camera *= Mat3::translation(Vec2(player.camera.x * 8.0f, player.camera.y * 8.0f)); // offset to middle of world
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer

  if(thunk_a_bunch){
    camera *= Mat3::translation(Vec2(
      float(blit::random() & 0x03) - 1.5f,
      float(blit::random() & 0x03) - 1.5f
    ));
    thunk_a_bunch--;
    vibration = thunk_a_bunch / 10.0f;
  }
}

uint8_t anim_player(uint8_t index, bool *forward, uint32_t direction) {
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
  uint8_t newSprite = index;
  bool animDirection = *forward;
  switch (index % 4) {
    case 0: 
      newSprite = offset + 1;
      *forward = true;
      break;
    case 1:
      newSprite = (animDirection) ? offset + 2 : offset + 0;
      break;
    case 2:
      newSprite = (animDirection) ? offset + 3 : offset + 1;
      break;
    case 3:
      newSprite = offset + 2;
      *forward = false;
      break;
  }
  if (newSprite > 15) {
    printf("Warning sprite %d out of range.", newSprite);
    newSprite = 0;
  }
  return newSprite;
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
  static uint32_t last_pacman_animation = time;
  static bool forwardPacmanAnimation = true;
  static uint32_t direction = Button::DPAD_DOWN;

  Point movement = Point(0, 0);

  if(!player.dead) {
    if(buttons & Button::DPAD_UP) {
      movement = Point(0,-1);
      direction = Button::DPAD_UP;
    } 
    else if(buttons & Button::DPAD_DOWN) {
      movement = Point(0,1);
      direction = Button::DPAD_DOWN;
    }
    else if(buttons & Button::DPAD_LEFT) {
      movement = Point(-1,0);
      direction = Button::DPAD_LEFT;
    }
    else if(buttons & Button::DPAD_RIGHT) {
      movement = Point(1,0);
      direction = Button::DPAD_RIGHT;
    }

    player.screen_location += movement;
    // Animate at 12fps.
    if (time - last_pacman_animation > (1000/60)) {
      last_pacman_animation = time;
      player.sprite = anim_player(player.sprite, &forwardPacmanAnimation, direction);
    }
  }

  update_camera(time);
}
