asciimate
=====
Play asciimation (ASCII animation) files.

Usage
-----
```sh
asciimate FILE
```

Supported options:
 * -f, --fps: Set the frames per second (FPS) for the animation (Default: 15). A higher number results in faster playback, while a lower number results in slower playback.
 * -r, --resolution: Try to set the display resolution to the specified width and height (e.g. 800x600). The format is WIDTHxHEIGHT@DEPTH, where WIDTH is the width in pixels, HEIGHT is the height in pixels, and DEPTH is the color depth in bits per pixel (e.g. 32).
 * -s, --scale: Set a resolution scaling factor (Default: 1.0). The factor must be less than or equal to 1.0, and it scales the display of the animation accordingly. For example, a factor of 0.5 will render the animation at half the original size and scale it up to fit the display resolution. This might improve performance on lower-end hardware at the cost of image quality.
 * -h, --help: Show this help message and exit.

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