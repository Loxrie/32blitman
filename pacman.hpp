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

    float speed;
    float dots_speed;
    float fright_speed;
    float fright_dots_speed;

    void power_timer_callback(blit::Timer &t);
    blit::Timer power_timer;
    bool power;
    
    blit::Vec2 desired_movement;
    uint32_t desired_direction;
    entityType moving_to;
    uint8_t sprite;
    bool animation_direction;
    uint32_t score;
    uint32_t level;
    uint32_t lives;

    Pacman();
    void init(LevelData ld);
    void new_game();
    bool is_pilled_up();

    void animate();
    void update(uint32_t time);
    void render();
    
    // How do I make this not be here?
    std::function<void(blit::Point)> collision_detection;
};

#endif
