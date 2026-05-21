play
=====
Play wave files via the sound card.

Usage
-----
```
play [OPTION]... FILE
```

Supported options:
* -h, --help: Show this help message and exit.

Press Escape to stop the playback of the wave file.

A supported sound card needs to be installed for sound to be actually played.
Currently, hhuOS only supports SoundBlaster 16 (or older) cards.

The wave is expected to contain mono 8-bit samples at 22050 Hz.
Other formats might be supported in the future, once the audio subsystem is enhanced.

Examples
--------
```
[/]> play /user/bug/music.wav
```