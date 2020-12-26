#include "32blit.hpp"
#include "const.hpp"
#include "main.hpp"

#ifndef PACMAN_H
#define PACMAN_H

struct Pacman {
    blit::Size size;
    blit::Vec2 location;
    blit::Vec2 movement;
    uint32_t direction;

    blit::Vec2 desired_movement;
    uint32_t desired_direction;
    entityType moving_to;
    uint8_t sprite;
    bool animation_direction;
    uint32_t score;
    uint32_t level;
    uint32_t lives;

    Pacman();
    void anim_player();
    void update(uint32_t time);
    // How do I make this not be here?
    std::function<void(blit::Point)> collision_detection;
};

#endif
