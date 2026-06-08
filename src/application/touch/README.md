touch
=====
Create files if they do not exist.

Usage
-----
```
touch [OPTION]... FILE...
```

Supported options:
* -h, --help: Show this help message and exit.

All given FILEs are created if they do not exist.
The created files are empty.
Already existing files are not changed.

Examples
--------
```
[/] mkdir test
[/] cd test
[/] touch file1 file2
[/] ls
file1  file2
[/] touch file2 file3
[/] ls
file1  file2  file3
```