asciimate
=====
Play asciimation (ASCII animation) files in the terminal.

Usage
-----
```sh
asciimate FILE
```

Supported options:
 * -f, --fps: Set the frames per second (FPS) for the animation (Default: 15). A higher number results in faster playback, while a lower number results in slower playback.
 * -h, --help: Show this help message and exit.

Press Escape to stop the playback of the asciimation.

Asciimation files are text files containing frames of ASCII art.
The format was invented by Simon Jansen and is used in his Star Wars asciimation project (https://www.asciimation.co.nz/).  
It works by placing frames of ASCII art one after another in a text file, separated by a delay number, which indicates how long to wait before displaying the next frame.  
For the hhuOS implementation, the format is slightly modified to include a header line at the beginning of the file, which specifies the width and height of the frames.

During a normal build, the Star Wars asciimation files are downloaded and included in the disk image at `/user/asciimation/`.
A redistributable build of hhuOS (e.g. from GitHub Releases) does not include the asciimation files.

Examples
--------
```sh
[/]> asciimate /user/asciimation/star_wars.txt
```