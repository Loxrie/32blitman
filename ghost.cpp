#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);
  location = Vec2((19*8)+4,15*8);
  target = Vec2(player.location);
  decision_made = true;
  desired_movement = Vec2(-1,0);
  desired_direction = Button::DPAD_LEFT;

  sprite = 0;

  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
        this->moving_to = entityType::WALL;
    }
  };
}

struct CollMap {
  uint32_t direction;
  Point offset;
};

// Buggy, fails with equidistance.
uint32_t Ghost::target_to_dpad(uint32_t where_did_we_come_from) {
  /**
   * https://gameinternals.com/understanding-pac-man-ghost-behavior
   *
   * When a decision about which direction to turn is necessary, 
   * the choice is made based on which tile adjoining the intersection 
   * will put the ghost nearest to its target tile, measured in a straight line.
   * The distance from every possibility to the target tile is measured, and whichever
   * tile is closest to the target will be selected. 
   */

   /**
    *  Our tiles are somewhat offset from the real game world, upper left.
    *  Ideally we translate this to screen pixels and calculate with that,
    *  offsetting to match the arcade machine.
    *
    *  For now, we kludge it, so right and down are 2 offset not 1.
    */
  uint32_t where_will_we_go = 0;
  Point decision_tile = tile(location);
  Point target_tile = tile(target);
  std::vector<CollMap> cotton_eye_joe(3);
  switch(where_did_we_come_from) {
    // Assign Left, Right, Down, Up
    case Button::DPAD_LEFT:
    // We came from the right.. etc.
    cotton_eye_joe.assign({ 
      {Button::DPAD_LEFT,Point(-1,0)},
      {Button::DPAD_DOWN,Point(0,2)},
      {Button::DPAD_UP,Point(0, -1)}
    });
    break;
  case Button::DPAD_RIGHT:
    cotton_eye_joe.assign({
      {Button::DPAD_RIGHT,Point(2,0)},
      {Button::DPAD_DOWN,Point(0,2)},
      {Button::DPAD_UP,Point(0, -1)}
    });
    break;
  case Button::DPAD_UP:
    cotton_eye_joe.assign({
      {Button::DPAD_LEFT,Point(-1,0)},
      {Button::DPAD_RIGHT,Point(2,0)},
      {Button::DPAD_UP,Point(0, 2)}
    });
    break;
  case Button::DPAD_DOWN:
    cotton_eye_joe.assign({
      {Button::DPAD_LEFT,Point(-1,0)},
      {Button::DPAD_RIGHT,Point(2,0)},
      {Button::DPAD_DOWN,Point(0, -1)}
    });
    break;
  }

  float min_distance = 600.f;
  uint32_t min_direction = 0;
  for (CollMap &p : cotton_eye_joe) {
    Point source_tile = (decision_tile + p.offset);
    float distance = (Vec2(target_tile.x,target_tile.y) - Vec2(source_tile.x,source_tile.y)).length();
    if (distance < min_distance) {
      min_distance = distance;
      min_direction = p.direction;
    }
  }

  return min_direction;
}
  
void Ghost::update(uint32_t time) {
  Vec2 t_location = Vec2(location.x,location.y);
  moving_to = entityType::NOTHING;
  Rect bounds_lr;
  target = player.location;

  uint32_t flags = map.get_flags(tile(location));
  if (!decision_made && flags & entityType::JUNCTION) {
    decision_made = true;
    desired_direction = target_to_dpad(desired_direction);
    switch(desired_direction) {
      // Possible new direction.
      case Button::DPAD_UP:
        desired_movement = Vec2(0,-1);
        desired_direction = Button::DPAD_UP;
        break;
      case Button::DPAD_DOWN:
        desired_movement = Vec2(0,1);
        desired_direction = Button::DPAD_DOWN;
        break;
      case Button::DPAD_LEFT:
        desired_movement = Vec2(-1,0);
        desired_direction = Button::DPAD_LEFT;
        break;
      case Button::DPAD_RIGHT:
        desired_movement = Vec2(1,0);
        desired_direction = Button::DPAD_RIGHT;
        break;
    }
    printf("Decision made changing direction to %d.\n", desired_direction);
  } else if (decision_made && flags == 0) {
    decision_made = false;
  }

  // See if new input direction is valid.
  t_location += desired_movement;
  bounds_lr = feet(t_location, size);
  map.tiles_in_rect(bounds_lr, collision_detection);

  if (moving_to == entityType::NOTHING) {
    location = t_location;
  } else if (flags == 0) {
    moving_to = entityType::NOTHING;
    // White ghosts can't jump.
    desired_movement = Vec2(0,1);
    map.tiles_in_rect(feet(location + desired_movement, size), collision_detection);
    if (moving_to == entityType::NOTHING) {
      location += Vec2(0,1);
      return;
    }
    desired_movement = Vec2(-1, 0);
    map.tiles_in_rect(feet(location + desired_movement, size), collision_detection);
    if (moving_to == entityType::NOTHING) {
      location += Vec2(-1,0);
      return;
    }
    desired_movement = Vec2(1,0);
    map.tiles_in_rect(feet(location + desired_movement, size), collision_detection);
    if (moving_to == entityType::NOTHING) {
      location += Vec2(1,0);
      return;
    }
  }
}