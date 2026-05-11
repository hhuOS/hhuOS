peanut-gb
=====
Play Game Boy games using the [Peanut-GB](https://github.com/deltabeard/Peanut-GB) emulator.

Usage
-----
```
peanut-gb [OPTION]... FILE
```

Supported options:
* -s, --save FILE: Load a save game from the specified FILE.
* -h, --help: Show this help message and exit.


Key mappings
-----
The emulator uses the following key mappings:
 * Up/Down/Left/Right: Arrow keys
 * A/B: J/K
 * Start: Space
 * Select: Enter

The following keys can be used to change configuration while the emulator is running:
 * F1: Increase screen scale
 * F2: Decrease screen scale
 * F3: Cycle through color palettes
 * F4: Reset color palette
 * Escape: Exit the emulator

Color Palettes
-----
The emulator only plays original Game Boy games (no color) but supports the color palettes that the Game Boy Color used to colorize original Game Boy games.
If a color palette exists for a specific game, it is automatically used. F3 can be used to cycle through color palettes (like the Game Boy Color supported using button combinations).

Save Games
-----
If the emulated game has a cartridge RAM chip, it is emulated correctly and its content is written in to a file in the current directory on exit.
This file is automatically loaded the next time the game is started in this directory.
The save file path can be overwritten using the `--save` option.

Audio
-----
The emulator supports audio playback through the [minigb_apu](https://github.com/deltabeard/Peanut-GB/tree/master/examples/sdl2/minigb_apu) library.