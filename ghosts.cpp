#include "ghosts.hpp"
#include "ghost.hpp"
#include "main.hpp"
#include "const.hpp"

using namespace blit;

std::vector<Ghost *> ghosts;

std::vector<ghostState> cycleStates = {
  ghostState::SCATTER,
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE
};

uint8_t blinky_cycle_index = 0;
uint8_t pinky_cycle_index = 0;
uint8_t inky_cycle_index = 0;
uint8_t clyde_cycle_index = 0;

void manage_ghost_move_state(Timer &t, Ghost *g, uint8_t *cycle_index) {
  ghostState nextState = cycleStates[*cycle_index];
  if (*cycle_index != 0) {
    printf("%s::manage_ghost_move_state forcing direction change.\n", g->name.c_str());
    g->forced_direction_change = true;
  }
  g->set_move_state(nextState);
  if (*cycle_index < cycleTimes.size()) {
    int8_t next_cycle_time = cycleTimes[*cycle_index];
    if (next_cycle_time != -1) {
      // Setup next ghost cycle.
      g->move_cycle_timer.duration = cycleTimes[*cycle_index];
      (*cycle_index)++;
    } else {
      g->move_cycle_timer.stop();
    }
  }
}

void blinky_cycle_timer_callback(Timer &t) {
  manage_ghost_move_state(t, blinky, &blinky_cycle_index);
}

void pinky_cycle_timer_callback(Timer &t) {
  manage_ghost_move_state(t, pinky, &pinky_cycle_index);
}

void inky_cycle_timer_callback(Timer &t) {
  manage_ghost_move_state(t, inky, &inky_cycle_index);
}

void clyde_cycle_timer_callback(Timer &t) {
  manage_ghost_move_state(t, clyde, &clyde_cycle_index);
}

void Ghosts::init(uint8_t current_level) {
  blinky->init(level_data[current_level]);
  pinky->init(level_data[current_level]);
  inky->init(level_data[current_level]);
  clyde->init(level_data[current_level]);
}

void Ghosts::move_start() {
  for (auto ghost : ghosts) {
    ghost->move_cycle_timer.duration = 10;
    ghost->move_cycle_timer.start();
  }
}

void Ghosts::move_pause() {
  for (auto ghost : ghosts) {
    ghost->move_cycle_timer.pause();
  }
}

void Ghosts::move_resume() {
  for (auto ghost : ghosts) {
    // Don't resume move cycle timer unless it is paused.
    if ((ghost->state & ghostState::FRIGHTENED) == 0) {

      ghost->move_cycle_timer.start();
    }
  }
}

void Ghosts::move_reset_and_pause() {
  for(auto ghost: ghosts) {
    ghost->move_cycle_timer.stop();
    // Needs integrating into level_data;
    ghost->move_cycle_timer.duration = 7000;
  }
}

void Ghosts::set_state(uint8_t s) {
  for (auto ghost : ghosts) {
    ghost->set_state(s);
  }
}

void Ghosts::clear_state(uint8_t s) {
  for (auto ghost: ghosts) {

    ghost->clear_state(s);
  }
}

bool Ghosts::update(uint32_t t) {
  bool pacman_eaten = false;
  Point pacman_pt = tile(player->location);
  for (auto ghost : ghosts) {
    ghost->update(t);
    // Odd that pacman can pass through ghosts
    // way more often than the arcade game.
    Point ghost_pt = tile(ghost->location);
    if (pacman_pt == ghost_pt) {
      if (ghost->edible()) {
        ghost->set_state(ghostState::EATEN);
        player->score += 100;
      } else if (!ghost->eaten()) {
        pacman_eaten = true;
      }
    }
  }
  if (pacman_eaten) {
    Ghosts::move_pause();
  }
  return pacman_eaten;
}

void Blinky::init(LevelData ld) {
  // When power pill running low sub this in for animation 9.
  //   ghostAnims[9] = Rect(10,12,2,2);
  target_offset = 0;
  location = Point(19 * 8 + 4, 15 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER;
}

Blinky::Blinky() {
  name = "Blinky";
  anim_offset = 0;
  sprite = anim_offset;
  scatter_target = Point(31 * 8, 1 * 8);
  move_cycle_timer.init(blinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Pinky::init(LevelData ld) {
  target_offset = 4;
  location = Point( 19 * 8 + 4, 18 * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Pinky::Pinky() {
  name = "Pinky";
  anim_offset = 8;
  sprite = anim_offset;
  scatter_target = Point( 9 * 8, 1 * 8);
  move_cycle_timer.init(pinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Inky::init(LevelData ld) {
  target_offset = 2;
  location = Point( 17 * 8 + 4, 18 * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Inky::Inky() {
  name = "Inky";
  anim_offset = 16;
  sprite = anim_offset;
  scatter_target = Point( 34 * 8, 37 * 8);
  move_cycle_timer.init(inky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Clyde::init(LevelData ld) {  
  // Wrong but it'll do for now.
  target_offset = -4;
  location = Point( 21 * 8 + 4, 18 * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;
  
  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Clyde::Clyde() {
  name = "Clyde";
  anim_offset = 24;
  sprite = anim_offset;
  scatter_target = Point( 6 * 8, 37 * 8);
  move_cycle_timer.init(clyde_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}