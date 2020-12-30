#include "ghosts.hpp"
#include "ghost.hpp"
#include "main.hpp"
#include "const.hpp"

using namespace blit;

// First level scatter chase cycle.
// Scatter for 7 seconds, then Chase for 20 seconds.
// Scatter for 7 seconds, then Chase for 20 seconds.
// Scatter for 5 seconds, then Chase for 20 seconds.
// Scatter for 5 seconds, then switch to Chase mode permanently.
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
  printf("manage_ghost_move_state setting state %d for ghost %s\n", nextState, g->name.c_str());
  g->set_move_state(nextState);
  if (*cycle_index < cycleTimes.size()) {
    int8_t next_cycle_time = cycleTimes[*cycle_index];
    if (next_cycle_time != -1) {
      // Setup next ghost cycle.
      printf("manage_ghost_move_state %s timer pre  dur - %d started - %d loops - %d state - %d\n", g->name.c_str(), g->move_cycle_timer.duration, g->move_cycle_timer.started, g->move_cycle_timer.loops, g->move_cycle_timer.state);
      g->move_cycle_timer.duration = cycleTimes[*cycle_index] * 1000;
      printf("manage_ghost_move_state %s timer post dur - %d started - %d loops - %d state - %d\n", g->name.c_str(), g->move_cycle_timer.duration, g->move_cycle_timer.started, g->move_cycle_timer.loops, g->move_cycle_timer.state);
      *cycle_index += 1;
    } else {
      printf("manage_ghost_move_state %s stopped.\n", g->name.c_str());
      g->move_cycle_timer.stop();
    }
  }
}

void blinky_cycle_timer_callback(Timer &t) {
  printf("blinky::move_cycle_timer_callback fired %d.\n", blinky_cycle_index);
  manage_ghost_move_state(t, blinky, &blinky_cycle_index);
}

void pinky_cycle_timer_callback(Timer &t) {
  printf("pinky::move_cycle_timer_callback fired %d.\n", pinky_cycle_index);
  manage_ghost_move_state(t, pinky, &pinky_cycle_index);
}

void inky_cycle_timer_callback(Timer &t) {
  printf("inky::move_cycle_timer_callback fired %d.\n", inky_cycle_index);
  manage_ghost_move_state(t, inky, &inky_cycle_index);
}

void clyde_cycle_timer_callback(Timer &t) {
  printf("clude::move_cycle_timer_callback fired %d.\n", clyde_cycle_index);
  manage_ghost_move_state(t, clyde, &clyde_cycle_index);
}

void Blinky::init() {
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

  move_cycle_timer.stop();

  // When power pill running low sub this in for animation 9.
  //   ghostAnims[9] = Rect(10,12,2,2);
  target_offset = 0;
  location = Point((19 * 8) + 4, 15 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;
  state = ghostState::SCATTER;
}

Blinky::Blinky() {
  name = "Blinky";
  scatter_target = Point(31 * 8, 1 * 8);
  move_cycle_timer.init(blinky_cycle_timer_callback, 7000, -1);
  init();
}

void Pinky::init() {
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

  move_cycle_timer.stop();

  target_offset = 4;
  location = Point( (18 * 8), 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::SCATTER | ghostState::RESTING | ghostState::LEAVING;
}

Pinky::Pinky() {
  name = "Pinky";
  scatter_target = Point( (9 * 8), 1 * 8);
  move_cycle_timer.init(pinky_cycle_timer_callback, 7000, -1);
  init();
}

void Inky::init() {
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

  move_cycle_timer.stop();

  target_offset = 2;
  location = Point( (19 * 8) + 4, 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Inky::Inky() {
  name = "Inky";
  scatter_target = Point( 34 * 8, 37 * 8);
  move_cycle_timer.init(inky_cycle_timer_callback, 7000, -1);
  init();
}

void Clyde::init() {
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
  
  move_cycle_timer.stop();

  // Wrong but it'll do for now.
  target_offset = -4;
  location = Point( (21 * 8), 18 * 8);
  direction = Button::DPAD_LEFT;
  desired_direction = direction;
  speed = 0.75f;

  state = ghostState::SCATTER | ghostState::RESTING;
}

Clyde::Clyde() {
  name = "Clyde";
  scatter_target = Point( 6 * 8, 37 * 8);
  move_cycle_timer.init(clyde_cycle_timer_callback, 7000, -1);
  init();
}