#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

const uint8_t house_flags = ghostState::RESTING | ghostState::ARRIVING | ghostState::LEAVING;

const uint8_t tunnel_entity_flags = entityType::PORTAL | entityType::TUNNEL;

const blit::Rect ghostAnims[40] = {
  // Blinky
  blit::Rect(0,0,2,2),
  blit::Rect(2,0,2,2),
  blit::Rect(4,0,2,2),
  blit::Rect(6,0,2,2),
  blit::Rect(8,0,2,2),
  blit::Rect(10,0,2,2),
  blit::Rect(12,0,2,2),
  blit::Rect(14,0,2,2),
  // Pinky
  blit::Rect(0,2,2,2),
  blit::Rect(2,2,2,2),
  blit::Rect(4,2,2,2),
  blit::Rect(6,2,2,2),
  blit::Rect(8,2,2,2),
  blit::Rect(10,2,2,2),
  blit::Rect(12,2,2,2),
  blit::Rect(14,2,2,2),
  // Inky
  blit::Rect(0,4,2,2),
  blit::Rect(2,4,2,2),
  blit::Rect(4,4,2,2),
  blit::Rect(6,4,2,2),
  blit::Rect(8,4,2,2),
  blit::Rect(10,4,2,2),
  blit::Rect(12,4,2,2),
  blit::Rect(14,4,2,2),
  // Clyde      
  blit::Rect(0,6,2,2),
  blit::Rect(2,6,2,2),
  blit::Rect(4,6,2,2),
  blit::Rect(6,6,2,2),
  blit::Rect(8,6,2,2),
  blit::Rect(10,6,2,2),
  blit::Rect(12,6,2,2),
  blit::Rect(14,6,2,2),
  // Scared
  blit::Rect(0,8,2,2),
  blit::Rect(2,8,2,2),
  // Flashing
  blit::Rect(0,8,2,2),
  blit::Rect(4,8,2,2),
  // Eyes
  blit::Rect(6,8,2,2),
  blit::Rect(8,8,2,2),
  blit::Rect(10,8,2,2),
  blit::Rect(12,8,2,2),
};

struct Ghost {
  public:
    std::string name;
    
    blit::Size size;
    uint8_t anim_offset;

    blit::Point location;

    int32_t target_offset;
    blit::Point target_tile;

    uint32_t direction;
    uint32_t desired_direction;
    bool forced_direction_change;

    blit::Point scatter_target;

    uint32_t last_update;

    float speed;
    float tunnel_speed;
    float fright_speed;

    uint8_t state;

    uint8_t moving_to;

    uint8_t sprite;

    blit::Timer move_cycle_timer;

    Ghost();
  
    uint32_t invertDirection();
    blit::Rect center(blit::Point pos);
    uint32_t direction_to_target(blit::Point target);
    uint32_t random_direction();

    bool edible();
    bool eaten();
    
    void set_move_state(ghostState s);
    void clear_state(uint8_t s);
    void set_state(uint8_t s);
    void handle_house();
    void handle_timers();
    void update(uint32_t time);
    void animate(uint32_t t);
    void render();
};

#endif