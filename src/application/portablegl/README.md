portablegl
=====
PortableGL demo application.

Usage
-----
```
portablegl [OPTION]... DEMO
```

Supported options:
* -r, --resolution WIDTHxHEIGHT\@BPP: Set the display resolution before starting the demo (may not have any effect depending on the display driver).
* -s, --scale SCALE: A floating point number in (0,1] used to scale the demo.
  The demo is rendered internally at the lower-scaled resolution and then scaled up to the actual resolution
  (e.g., a scale of 0.5 will cause the demo to be rendered at half the actual resolution).
  This option can help with performance on low-end devices.
* -h, --help: Show this help message and exit.

This application uses the [PortableGL](https://github.com/rswinkle/PortableGL/) library to render OpenGL demos.

Supported demos:
* info: Show OpenGL information.
* triangle: Render a rotating triangle with color blending.
* gears: Render spinning gear wheels, similar to the famous `glxgears` application (Based on: [https://github.com/rswinkle/PortableGL/blob/master/examples/classic/gears.c](https://github.com/rswinkle/PortableGL/blob/master/examples/classic/gears.c)).

Examples
--------

### Triangle Demo:
![Screenshot](portablegl1.png)

### Gears Demo:
![Screenshot](portablegl2.png)
