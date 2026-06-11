unmount
=====
Unmount the filesystem from the specified directory.

Usage
-----
```
unmount [OPTION]... PATH
```

Supported options:
* -h, --help: Show this help message and exit.

The filesystem mounted at PATH will be unmounted.
If no filesystem is mounted at PATH, nothing happens.

Examples
--------
```
[/]> ls /media/floppy
bin/ books/
[/]> mount -t Filesystem::Fat::FatDriver floppy0 /media/floppy
mount: Failed to mount 'floppy0'!
[/]> unmount /media/floppy
[/]> ls /media/floppy

[/]> mount -t Filesystem::Fat::FatDriver floppy0 /media/floppy
[/]> ls /media/floppy
bin/ books/
```