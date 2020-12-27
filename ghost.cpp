#include "ghost.hpp"
#include "main.hpp"

using namespace blit;

Ghost::Ghost() {
  size = Size(14, 14);
  location = Point((19*8)+4,15*8);
  target = Vec2(player.location);
  decision_made = false;
  direction = 0;
  movement = Vec2(-1,0);
  desired_movement = movement;

  sprite = 0;

  collision_detection = [this](Point tile_pt) -> void {
    printf("Ghost::ghost collision_detection checking %ld:%ld.\n", tile_pt.x, tile_pt.y);
    if(map.has_flag(tile_pt, entityType::WALL)) {
        printf("Ghost::ghost collision_detection tile number %d has flags %d.\n", map.layers["background"].tile_at(tile_pt), map.get_flags(tile_pt));
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
  uint32_t index = decision_tile.y * level_width + decision_tile.x;
  auto search = mapOfJunctions.find(index);
  if (search == mapOfJunctions.end()) {
    printf("Ghost::vector_to_target freezing ghost, no mapping for %ld:%ld index %ld.\n", decision_tile.x, decision_tile.y, index);
    return Vec2(0,0);
  }

  float min_distance = 600.f;
  Vec2 min_vector;
  for (Vec2 vector : search->second) {
    if (vector.x == inverted_vector.x && vector.y == inverted_vector.y) {
      printf("Ghost::vector_to_target Skipping %.0f:%0.f.\n", (double)vector.x, (double)vector.y);
      continue;
    }

    Point source_tile = Point(decision_tile.x + vector.x, decision_tile.y + vector.y);
    float distance = fabs(Vec2(target_tile.x - source_tile.x, target_tile.y - source_tile.y).length());
    printf("Ghost::vector_to_target checking point %ld:%ld distance %3.3f min is %3.3f.\n", source_tile.x, source_tile.y, (double)distance, (double)min_distance);    
    if (distance < min_distance) {
      min_distance = distance;
      min_vector = vector;
    }
  }

  return min_vector;
}
  
void Ghost::update(uint32_t time) {
  // Only update every 8 pixels.
  if (location.x % 8 != 0 || location.y % 8 != 0) {
    printf("Inbetween tiles, floating.\n");
    location += movement;
    return;
  }

  moving_to = entityType::NOTHING;
  Rect bounds_lr;
  target = player.location;
  float vector_divisor = 1.0f;

  Point tile_pt = tile(location);
  uint32_t flags = map.get_flags(tile_pt);
  print_flags(flags);
  if (flags & entityType::WARP) {
    vector_divisor = 2.0f;
  }

  if (!decision_made && flags & entityType::JUNCTION) {
    decision_made = true;
    desired_movement = vector_to_target();
    printf("Ghost::update decision move to %.0f:%0.f\n", (double)desired_movement.x, (double)desired_movement.y);
  }

  // See if new input direction is valid.
  if (decision_made) {
    bounds_lr = footprint(location + (desired_movement * 8), size);
    map.tiles_in_rect(bounds_lr, collision_detection);
    if (moving_to == entityType::NOTHING) {
      movement = desired_movement;
      printf("Ghost::update desired move to %0.f:%0.f\n", (double)movement.x, (double)movement.y);
      decision_made = false;
      location += desired_movement/vector_divisor;
      return;
    } 
  }
  
  // Use pre-existing movement.
  moving_to = entityType::NOTHING;
  printf("Ghost::update Calculating bounds with %ld:%ld\n", location.x, location.y);
  bounds_lr = footprint(location + (movement * 8), size);
  map.tiles_in_rect(bounds_lr, collision_detection);
  printf("Ghost::update tiles in rect %ld:%ld %ld:%ld found %d.\n", bounds_lr.x/8, bounds_lr.y/8, bounds_lr.w, bounds_lr.h, moving_to);
  if (moving_to == entityType::NOTHING) {
    printf("Ghost::update move to %0.f:%0.f\n", (double)movement.x, (double)movement.y);
    location += movement/vector_divisor;
    return;
  } 
  
  // Still can't move, try turning a corner.
  if (moving_to == entityType::WALL && !(flags & entityType::JUNCTION)) {
    Vec2 inverted_vector = Vec2(-movement.x, -movement.y);
    printf("Movement is %.0f:%0.f, so we don't want %.0f:%.0f as possible direction.\n", (double)movement.x, (double)movement.y, (double)inverted_vector.x, (double)inverted_vector.y);
    Point stuck_at = tile(location);
    uint32_t index = stuck_at.y * level_width + stuck_at.x;
    auto search = mapOfJunctions.find(index);
    if (search == mapOfJunctions.end()) {
      printf("No mapping for %ld:%ld index %ld.\n", stuck_at.x, stuck_at.y, index);
      return;
    }

    for (Vec2 vector : search->second) {
      if (vector.x == inverted_vector.x && vector.y == inverted_vector.y) {
        printf("Skipping escape route from %ld:%ld with vector %.0f:%.0f.\n", stuck_at.x, stuck_at.y, (double)vector.x, (double)vector.y);
        continue;
      }
      printf("Checking escape route from %ld:%ld with vector %.0f:%.0f.\n", stuck_at.x, stuck_at.y, (double)vector.x, (double)vector.y);
      moving_to = entityType::NOTHING;
      map.tiles_in_rect(footprint(location + (vector * 8), size), collision_detection);
      if (moving_to == entityType::NOTHING) {
        // This slows down ghost on corner?
        desired_movement = vector;
        decision_made = true;
        break;
      }
    }
  }
}