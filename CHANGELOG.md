
31/12/20

- Ghosts managed code moved to static Ghosts methods.
- Removed unused level_data, reused var name for difficulty level mapping
- Next level proceeds to next difficulty in level mapping.
- Remove "pause" feature, 32blit has a perfectly servicable pause button.
- Scatter chase cycle implemented.  Breaking change, reliant on blit::Timer pause.
- Difficult level mapping for ghost speed, pacman speed, and future bonus fruit.
- Remove pause, the 32blit hardware has a pause option.
- Move ghost management loops out to Ghosts struct and statics.
- Fix resuming ghost move cycle bug on pill expiry.
