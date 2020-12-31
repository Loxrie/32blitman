#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

extern blit::Rect ghostAnims[12];

struct Ghost {
  public:
    std::string name;
    
    blit::Rect ghostAnims[14];

    blit::Size size;
    blit::Point location;

    int32_t target_offset;
    blit::Point target_tile;

    uint32_t direction;
    uint32_t desired_direction;

    blit::Point scatter_target;

    uint32_t last_update;

    float speed;
    float tunnel_speed;
    float fright_speed;

    uint8_t state;

    entityType moving_to;

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
    void animate();
    void render();

    std::function<void(blit::Point)> collision_detection;
};

#endif