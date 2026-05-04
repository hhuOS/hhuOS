mount
=====
Mount a filesystem at a specified directory.

Usage
-----
```
mkdir [OPTION]... DEVICE PATH
```

Supported options:
* -t, --type: The filesystem driver to use for the mount operation (e.g. Filesystem::Fat::FatDriver). This option is required.
* -h, --help: Show this help message and exit.

Mount the filesystem on the specified DEVICE to the specified PATH.  

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