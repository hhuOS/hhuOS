mkdir
=====
Create directories.

Usage
-----
```
mkdir [OPTION]... DIRECTORY...
```

Supported options:
* -v, --verbose: Print a message for each created directory.
* -h, --help: Show this help message and exit.

Create the specified DIRECTORY(ies), if they do not already exist.
The parent directories must already exist in order to create a new directory.

Examples
--------
```
[/]> mkdir dir1
[/]> cd dir1
[/dir1]> mkdir subdir1
[/dir1]> ls
subdir1/
[/dir1]> mkdir -v subdir2 subdir3
mkdir: created directory 'subdir2'
mkdir: created directory 'subdir3'
[/dir1]> ls
subdir1/  subdir2/  subdir3/
```