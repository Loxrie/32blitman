02/01/21

- Ghost house changes, more like arcade game.  Up/Down rather than Left/Right.  Speed is slower.
- Major rewrite.  Having level and pills as separate sprite maps killed render time.

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
