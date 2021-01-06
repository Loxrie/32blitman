06/01/21

- Ghosts targets corrected. Pinky 4 tiles ahead. Inky "double" the vector from blinky to pacman.  Clyde fluctuates between pacman and scatter corner, though it's not strictly a "cardinal" 8 tiles.
- Fixed a bunch of issues around level/game reset.

05/01/21

- Ghosts look in desired_direction, e.g. so they can face down just before turn.
- Behaviour is now based on tile change rather than precise alignment with a set point in a tile.
- Ghosts can now reverse anywhere when entering new tile if flag is set.  Removed "junction" requirement.
- Ghosts can now reverse when they're eaten.
- Level wasn't being reset.
- Ghosts now reverse direction when a power dot is eaten.
- Pacman/Ghost speed is more appropriate now, based on a pool of "velocity" rather than limiting update speed.

03/01/21

- Animated eyes for eaten ghosts.
- Fruit
- Eaten ghosts in succession now gives appropriate points as does the first ghost.

02/01/21

- Add levels beyond 5.
- Release the ghosts if no pills are eaten in "x" seconds. 4 seconds, 3 seconds post level 5.
- Scatter/Cycle into level data.
- No ghost reversal on state change or house exit.
- Ghost house changes, more like arcade game.  Up/Down rather than Left/Right.  Speed is slower in ghost house.
- Major rewrite.  Having level and pills as separate sprite maps killed render time.
- Added metadata images.
- Ghosts exit house after life loss on the arcade style cycle of pills.

01/01/21

- Added lives counter.
- Flashing ghosts based on level warning flash count
- Score reset on new game.
- Fix camera to center more of the maze
- ^^ made pill drawing a bit wobbly, fix that by abandoning draw_layer and using another Mat3 transform.
- ^^ this may have made ghost/pacman drawing a little wobbly :-D
- Frightened randomisation was really broken, now fixed.

31/12/20

- Fix unable to restart from game over.
- Ghosts managed code moved to static Ghosts methods.
- Removed unused level_data, reused var name for difficulty level mapping
- Next level proceeds to next difficulty in level mapping.
- Remove "pause" feature, 32blit has a perfectly servicable pause button.
- Scatter chase cycle implemented.  Breaking change, reliant on blit::Timer pause.
- Difficult level mapping for ghost speed, pacman speed, and future bonus fruit.
- Remove pause, the 32blit hardware has a pause option.
- Move ghost management loops out to Ghosts struct and statics.
- Fix resuming ghost move cycle bug on pill expiry.
