#include "ghosts.hpp"
#include "ghost.hpp"
#include "main.hpp"
#include "const.hpp"

using namespace blit;

std::vector<Ghost *> ghosts;

// Level 1
// Scatter 7 -> Chase 20 -> Scatter 7 -> Chase 20 -> Scatter 5 -> Chase 20 -> Scatter 5 -> Chase
// Level 2 - 4
// Scatter 7 -> Chase 20 -> Scatter 7 -> Chase 20 -> Scatter 5 -> Chase 1033 -> Scatter 1/60 -> Chase
// Level 5+
// Scatter 5 -> Chase 20 -> Scatter 5 -> Chase 20 -> Scatter 5 -> Chase 1037 -> Scatter 1/60 -> Chase

std::vector<ghostState> cycleStates = {
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE,
  ghostState::SCATTER,
  ghostState::CHASE
};

std::vector<int8_t> cycleTimes = {
  20,
  7,
  20,
  5,
  20,
  5,
  -1
};

int8_t blinky_cycle_index = 0;
int8_t pinky_cycle_index = 0;
int8_t inky_cycle_index = 0;
int8_t clyde_cycle_index = 0;

void manage_ghost_move_state(Timer &t, Ghost *g, int8_t *cycle_index) {
  ghostState nextState = cycleStates[*cycle_index];
  printf("%s::manage_ghost_move_state cycle %d setting state %d.\n", g->name.c_str(),  *cycle_index, nextState);
  g->set_move_state(nextState);
  if (*cycle_index < cycleTimes.size()) {
    int8_t next_cycle_time = cycleTimes[*cycle_index];
    if (next_cycle_time != -1) {
      // Setup next ghost cycle.
      printf("%s::manage_ghost_move_state timer pre  dur - %d started - %d loops - %d state - %d\n", g->name.c_str(), g->move_cycle_timer.duration, g->move_cycle_timer.started, g->move_cycle_timer.loops, g->move_cycle_timer.state);
      g->move_cycle_timer.duration = cycleTimes[*cycle_index] * 1000;
      printf("%s::manage_ghost_move_state timer post dur - %d started - %d loops - %d state - %d\n", g->name.c_str(), g->move_cycle_timer.duration, g->move_cycle_timer.started, g->move_cycle_timer.loops, g->move_cycle_timer.state);
      (*cycle_index)++;
    } else {
      printf("%s::manage_ghost_move_state stopped.\n", g->name.c_str());
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
    printf("Ghosts::move_start Starting move cycle timer for %s\n", ghost->name.c_str());
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
      printf("Ghosts::resume_move %s move resume.\n", ghost->name.c_str());
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
    printf("Ghosts::clear_state %s state %d\n", ghost->name.c_str(), ghost->state);
    ghost->clear_state(s);
  }
}

bool Ghosts::update(uint32_t t) {
  bool pacman_eaten = false;
  Point pacman_pt = tile(player->location);
  for (auto ghost : ghosts) {
    ghost->update(t);
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
  ghostAnims[0] = Rect(0,4,2,2);
  ghostAnims[1] = Rect(2,4,2,2);
  ghostAnims[2] = Rect(4,4,2,2);
  ghostAnims[3] = Rect(6,4,2,2);
  ghostAnims[4] = Rect(8,4,2,2);
  ghostAnims[5] = Rect(10,4,2,2);
  ghostAnims[6] = Rect(12,4,2,2);
  ghostAnims[7] = Rect(14,4,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);

  // When power pill running low sub this in for animation 9.
  //   ghostAnims[9] = Rect(10,12,2,2);
  target_offset = 0;
  location = Point((19 * 8) + 4, 15 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER;
}

Blinky::Blinky() {
  name = "Blinky";
  scatter_target = Point(31 * 8, 1 * 8);
  move_cycle_timer.init(blinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Pinky::init(LevelData ld) {
  ghostAnims[0] = Rect(0,8,2,2);
  ghostAnims[1] = Rect(2,8,2,2);
  ghostAnims[2] = Rect(4,8,2,2);
  ghostAnims[3] = Rect(6,8,2,2);
  ghostAnims[4] = Rect(8,8,2,2);
  ghostAnims[5] = Rect(10,8,2,2);
  ghostAnims[6] = Rect(12,8,2,2);
  ghostAnims[7] = Rect(14,8,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);

  target_offset = 4;
  location = Point( (18 * 8), 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING | ghostState::LEAVING;
}

Pinky::Pinky() {
  name = "Pinky";
  scatter_target = Point( (9 * 8), 1 * 8);
  move_cycle_timer.init(pinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Inky::init(LevelData ld) {
  ghostAnims[0] = Rect(0,10,2,2);
  ghostAnims[1] = Rect(2,10,2,2);
  ghostAnims[2] = Rect(4,10,2,2);
  ghostAnims[3] = Rect(6,10,2,2);
  ghostAnims[4] = Rect(8,10,2,2);
  ghostAnims[5] = Rect(10,10,2,2);
  ghostAnims[6] = Rect(12,10,2,2);
  ghostAnims[7] = Rect(14,10,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(12,12,2,2);
  ghostAnims[10] = Rect(14,12,2,2);

  target_offset = 2;
  location = Point( (19 * 8) + 4, 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Inky::Inky() {
  name = "Inky";
  scatter_target = Point( 34 * 8, 37 * 8);
  move_cycle_timer.init(inky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

void Clyde::init(LevelData ld) {
  ghostAnims[0] = Rect(0,6,2,2);
  ghostAnims[1] = Rect(2,6,2,2);
  ghostAnims[2] = Rect(4,6,2,2);
  ghostAnims[3] = Rect(6,6,2,2);
  ghostAnims[4] = Rect(8,6,2,2);
  ghostAnims[5] = Rect(10,6,2,2);
  ghostAnims[6] = Rect(12,6,2,2);
  ghostAnims[7] = Rect(14,6,2,2);
  ghostAnims[8] = Rect(8,12,2,2);
  ghostAnims[9] = Rect(8,12,2,2);
  ghostAnims[11] = Rect(14,12,2,2);
  ghostAnims[10] = Rect(12,12,2,2);
  
  // Wrong but it'll do for now.
  target_offset = -4;
  location = Point( (21 * 8), 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  
  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Clyde::Clyde() {
  name = "Clyde";
  scatter_target = Point( 6 * 8, 37 * 8);
  move_cycle_timer.init(clyde_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}