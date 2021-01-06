#include "ghosts.hpp"
#include "const.hpp"
#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

std::vector<Ghost *> ghosts;

void manage_ghost_move_state(Timer &t, Ghost *g) {
  uint8_t cycle_index = ++g->cycle_index;
  ghostState nextState = (cycle_index % 2) == 0 ? ghostState::SCATTER : ghostState::CHASE;
  printf("%s::manage_ghost_move_state called index %d nexr state %d.\n", g->name.c_str(), cycle_index, nextState);
  if (cycle_index != 0) {
    printf("%s::manage_ghost_move_state forcing direction change.\n", g->name.c_str());
    g->forced_direction_change = true;
  }
  g->set_move_state(nextState);
  if (cycle_index < cycleTimes.size()) {
    int8_t next_cycle_time = cycleTimes[cycle_index];
    if (next_cycle_time != -1) {
      // Setup next ghost cycle.
      g->move_cycle_timer.duration = cycleTimes[cycle_index];
    } else {
      g->move_cycle_timer.stop();
    }
  }
}

void blinky_cycle_timer_callback(Timer &t) { manage_ghost_move_state(t, blinky); }

void pinky_cycle_timer_callback(Timer &t) { manage_ghost_move_state(t, pinky); }

void inky_cycle_timer_callback(Timer &t) { manage_ghost_move_state(t, inky); }

void clyde_cycle_timer_callback(Timer &t) { manage_ghost_move_state(t, clyde); }

void Ghosts::init(uint8_t current_level) {
  blinky->init(level_data[current_level]);
  pinky->init(level_data[current_level]);
  inky->init(level_data[current_level]);
  clyde->init(level_data[current_level]);
}

void Ghosts::move_start() {
  for (auto g : ghosts) {
    g->move_cycle_timer.start();
  }
}

void Ghosts::set_state(uint8_t s) {
  for (auto g : ghosts) {
    g->set_state(s);
  }
}

void Ghosts::clear_state(uint8_t s) {
  for (auto g : ghosts) {z
    g->clear_state(s);
  }
}

void Ghosts::power_pill_eaten() {
  for (auto g : ghosts) {
    uint32_t rd = g->invertDirection();
    g->set_state(ghostState::FRIGHTENED);
    g->direction = rd;
    g->desired_direction = rd;
  }
}

bool Ghosts::update(uint32_t t) {
  bool pacman_eaten = false;
  for (auto g : ghosts) {
    g->update(t);
    // Odd that pacman can pass through ghosts
    // way more often than the arcade game.
    if (player->tile == g->tile) {
      if (g->edible()) {
        g->set_state(ghostState::EATEN);
        player->score += 100 * pow(2, ghost_train);
        ghost_train++;
      } else if (!g->eaten()) {
        pacman_eaten = true;
      }
    }
  }
  return pacman_eaten;
}

void Blinky::init(LevelData ld) {
  // When power pill running low sub this in for animation 9.
  //   ghostAnims[9] = Rect(10,12,2,2);
  target_offset = 0;
  tile = Point(18, 15);
  location = Point(tile.x * 8 + 4, tile.y * 8);

  direction = Button::DPAD_LEFT;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;
  
  move_cycle_timer.stop();
  move_cycle_timer.duration = cycleTimes[cycle_index];
  state = (cycle_index % 2) == 0 ? ghostState::SCATTER : ghostState::CHASE;
  printf("%s::init state %d, cycle_index %d.\n", name.c_str(), state, cycle_index);
}

Blinky::Blinky() {
  name = "Blinky";
  anim_offset = 0;
  sprite = anim_offset;
  scatter_target = Point(31, 0);
  move_cycle_timer.init(blinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

Point Blinky::get_target() { return player->tile; }

void Pinky::init(LevelData ld) {
  target_offset = 4;
  tile = Point(19, 18);
  location = Point(tile.x * 8 + 4, tile.y * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  move_cycle_timer.stop();
  move_cycle_timer.duration = cycleTimes[cycle_index];

  state = (cycle_index % 2) == 0 ? ghostState::SCATTER : ghostState::CHASE;
  state |= ghostState::RESTING;
  printf("%s::init state %d, cycle_index %d.\n", name.c_str(), state, cycle_index);
}

Pinky::Pinky() {
  name = "Pinky";
  anim_offset = 8;
  sprite = anim_offset;
  scatter_target = Point(9, 0);
  move_cycle_timer.init(pinky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

Point Pinky::get_target() {
  Point target_tile = player->tile;
  switch (player->direction) {
  case Button::DPAD_LEFT:
    target_tile.x -= 4;
    break;
  case Button::DPAD_RIGHT:
    target_tile.x += 4;
    break;
  case Button::DPAD_UP:
    target_tile.y -= 4;
    break;
  case Button::DPAD_DOWN:
    target_tile.y += 4;
    break;
  }
  return target_tile;
}

void Inky::init(LevelData ld) {
  target_offset = 2;
  tile = Point(17, 18);
  location = Point(tile.x * 8 + 4, tile.y * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  move_cycle_timer.stop();
  move_cycle_timer.duration = cycleTimes[cycle_index];
  state = (cycle_index % 2) == 0 ? ghostState::SCATTER : ghostState::CHASE;
  state |= ghostState::RESTING;
  printf("%s::init state %d, cycle_index %d.\n", name.c_str(), state, cycle_index);
}

Inky::Inky() {
  name = "Inky";
  anim_offset = 16;
  sprite = anim_offset;
  scatter_target = Point(33, 36);
  move_cycle_timer.init(inky_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

Point Inky::get_target() {
  Point pacman_tile = player->tile;
  Point blinky_tile = blinky->tile;
  Vec2 vector = Vec2(pacman_tile.x - blinky_tile.x, pacman_tile.y - blinky_tile.y) * 2;
  return Point(pacman_tile.x + vector.x, pacman_tile.y + vector.y);
}

void Clyde::init(LevelData ld) {
  // Wrong but it'll do for now.
  target_offset = -4;
  tile = Point(21, 18);
  location = Point(tile.x * 8 + 4, tile.y * 8);
  direction = Button::DPAD_UP;
  desired_direction = direction;

  speed = ld.ghost_speed;
  tunnel_speed = ld.ghost_tunnel_speed;
  fright_speed = ld.ghost_fright_speed;

  move_cycle_timer.stop();
  move_cycle_timer.duration = cycleTimes[cycle_index];
  state = (cycle_index % 2) == 0 ? ghostState::SCATTER : ghostState::CHASE;
  state |= ghostState::RESTING;
  printf("%s::init state %d, cycle_index %d.\n", name.c_str(), state, cycle_index);
}

Clyde::Clyde() {
  name = "Clyde";
  anim_offset = 24;
  sprite = anim_offset;
  scatter_target = Point(6, 36);
  
  move_cycle_timer.init(clyde_cycle_timer_callback, 7000, -1);
  init(level_data[0]);
}

Point Clyde::get_target() {
  Point pacman_tile = player->tile;
  float distance = fabs(Vec2(pacman_tile.x - tile.x, pacman_tile.y - tile.y).length());
  if (distance > 8) {
    return player->tile;
  }
  return scatter_target;
}