beep
=====
Play beep file via the PC speaker.

Usage
-----
```
beep FILE
```

Supported options:
 * -h, --help: Show this help message and exit.

Press Escape to stop the playback of the beep file.

Beep files are text files containing a sequence of piano notes and their durations. The format is as follows:
```
FREQUENCY,DURATION
```
where FREQUENCY is the frequency of the note in Hz (e.g. 440 for A4), and DURATION is the duration of the note in milliseconds (e.g. 500 for half a second).
Each line in the file represents one note.

During a normal build, some beep files are generated and included in the disk image at `/user/beep/`.
The piano notes for generating the beep files are taken from the https://pianoletternotes.blogspot.com/.
A redistributable build of hhuOS (e.g. from GitHub Releases) does not include the asciimation files.

Examples
--------
```
[/]> beep /user/beep/tetris.beep
```