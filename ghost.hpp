#include "32blit.hpp"
#include "const.hpp"

#ifndef GHOST_H
#define GHOST_H

extern blit::Rect ghostAnims[12];

struct Ghost {
  public:
    std::string name;
    
    blit::Rect ghostAnims[12];

    blit::Size size;
    blit::Point location;

    int32_t target_offset;
    blit::Point target_tile;

    uint32_t direction;
    uint32_t desired_direction;

    uint32_t last_update;
    float speed;

    uint8_t state;

    entityType moving_to;

    uint8_t sprite;

    Ghost();

    uint32_t invertDirection();
    blit::Rect center(blit::Point pos);
    uint32_t direction_to_target(blit::Point target);
    uint32_t random_direction();

    bool eaten();
    void eaten(ghostState s);
    
    void set_move_state(ghostState s);
    void set_state(ghostState s);
    void update(uint32_t time);
    void animate();
    void render();

    std::function<void(blit::Point)> collision_detection;
};

#endif