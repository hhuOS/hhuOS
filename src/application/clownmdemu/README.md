peanut-gb
=====
Play Sega Mega Drive (Genesis) games using the [ClownMDEmu](https://github.com/Clownacy/clownmdemu-core) emulator.

Usage
-----
```
clownmdemu [OPTION]... FILE
```

Supported options:
* -r, --resolution WIDTHxHEIGHT\@BPP: Set the display resolution before starting the emulator (may not have any effect depending on the display driver).
* -h, --help: Show this help message and exit.

Key mappings
-----
The emulator uses the following key mappings:
 * Up/Down/Left/Right: Arrow keys
 * A/B/C: A/S/D
 * X/Y/Z: Y/X/C
 * Start: Space
 * Mode: Enter

The following keys can be used to change configuration while the emulator is running:
 * F1: Increase screen scale
 * F2: Decrease screen scale
 * Escape: Exit the emulator

Save Games
-----
Save games are supported and stored in a file defined by the emulator, depending on the game.

Audio
-----
Audio is currently not supported.