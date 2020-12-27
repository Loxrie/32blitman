#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(16, 16);
  location = Vec2((19*8)+4,15*8);
  target = Vec2(player.location);
  decision_made = true;
  direction = 0;
  movement = Vec2(-1,0);
  desired_movement = movement;

  sprite = 0;

  collision_detection = [this](Point tile_pt) -> void {
    if(map.has_flag(tile_pt, entityType::WALL)) {
        this->moving_to = entityType::WALL;
    }
  };
}

Vec2 Ghost::vector_to_target() {
  /**
   * https://gameinternals.com/understanding-pac-man-ghost-behavior
   *
   * When a decision about which direction to turn is necessary, 
   * the choice is made based on which tile adjoining the intersection 
   * will put the ghost nearest to its target tile, measured in a straight line.
   * The distance from every possibility to the target tile is measured, and whichever
   * tile is closest to the target will be selected. 
   */

  uint32_t where_will_we_go = 0;
  Point decision_tile = tile(location);
  Point target_tile = tile(target);
  
  Vec2 inverted_vector = Vec2(-movement.x, -movement.y);
  printf("Ghost::vector_to_target inverted movement %.0f:%0.f\n", inverted_vector.x, inverted_vector.y);
  uint32_t index = decision_tile.y * level_width + decision_tile.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    printf("Ghost::vector_to_target freezing ghost, no mapping for %d:%d index %d.\n", decision_tile.x, decision_tile.y, index);
    return Vec2(0,0);
  }

  float min_distance = 600.f;
  Vec2 min_vector;
  for (Vec2 vector : search->second) {
    if (vector.x == inverted_vector.x && vector.y == inverted_vector.y) {
      printf("Ghost::vector_to_target Skipping %.0f:%0.f.\n", vector.x, vector.y);
      continue;
    }
    
    Point source_tile = Point(decision_tile.x + vector.x, decision_tile.y + vector.y);
    float distance = fabs(Vec2(target_tile.x - source_tile.x, target_tile.y - source_tile.y).length());
    if (distance < min_distance) {
      min_distance = distance;
      min_vector = vector;
    }
  }

  return min_vector;
}
  
void Ghost::update(uint32_t time) {
  moving_to = entityType::NOTHING;
  Rect bounds_lr;
  target = player.location;
  float vector_divisor = 1.0f;

  uint32_t flags = map.get_flags(tile(location));

  if (flags & entityType::WARP) {
    vector_divisor = 2.0f;
  }


  if (!decision_made && flags & entityType::JUNCTION) {
    decision_made = true;
    desired_movement = vector_to_target();
    printf("Decision made to move %.0f:%0.f\n", desired_movement.x, desired_movement.y);
  }

  // See if new input direction is valid.
  bounds_lr = feet(location + desired_movement, size);
  map.tiles_in_rect(bounds_lr, collision_detection);

  if (moving_to == entityType::NOTHING) {
    movement = desired_movement;
    printf("Now moving to %0.f:%0.f\n", movement.x, movement.y);
    decision_made = false;
    location += desired_movement/vector_divisor;
    return;
  } 

  // Use pre-existing movement.
  moving_to = entityType::NOTHING;
  bounds_lr = feet(location + movement, size);
  map.tiles_in_rect(bounds_lr, collision_detection);

  if (moving_to == entityType::NOTHING) {
    location += movement/vector_divisor;
    return;
  } 
  
  // Still can't move, try turning a corner.
  if (moving_to == entityType::WALL && flags == 0) {
    Vec2 inverted_vector = Vec2(-movement.x, -movement.y);
    printf("Movement is %.0f:%0.f, so we don't want %.0f:%.0f as possible direction.\n", movement.x, movement.y, inverted_vector.x, inverted_vector.y);
    Point stuck_at = tile(location);
    uint32_t index = stuck_at.y * level_width + stuck_at.x;
    auto search = mapOfJunctions.find(index);
    if (search == mapOfJunctions.end()) {
      printf("No mapping for %d:%d index %d.\n", stuck_at.x, stuck_at.y, index);
      return;
    }

    for (Vec2 vector : search->second) {
      if (vector.x == inverted_vector.x && vector.y == inverted_vector.y) {
        printf("Skipping escape route from %d:%d with vector %.0f:%.0f.\n", stuck_at.x, stuck_at.y, vector.x, vector.y);
        continue;
      }
      printf("Checking escape route from %d:%d with vector %.0f:%.0f.\n", stuck_at.x, stuck_at.y, vector.x, vector.y);
      moving_to = entityType::NOTHING;
      map.tiles_in_rect(feet(location + vector, size), collision_detection);
      if (moving_to == entityType::NOTHING) {
        // This slows down ghost on corner?
        desired_movement = vector;
        break;
      }
    }
  }
}