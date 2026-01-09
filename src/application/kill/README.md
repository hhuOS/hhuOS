kill
=====
Terminate processes by PID.

Usage
-----
```sh
echo PID...
```

Supported options:
 * -h, --help: Show this help message and exit.

Examples
--------
This example starts the program `beep` to play a sound file via the PC Speaker in the background.
It then lists the running processes with `ps`, terminates the `beep` process using `kill`,
and lists the running processes again to confirm that `beep` has been terminated.
```sh
[/]> beep /user/beep/tetris.beep > /device/null
[/]> ps
PID   Threads Name
0     9       Kernel
1     1       /bin/shell
4     1       /bin/beep
5     1       /bin/ps
[/]> kill 4
[/]> ps
PID   Threads Name
0     9       Kernel
1     1       /bin/shell
5     1       /bin/ps
```