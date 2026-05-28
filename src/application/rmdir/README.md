rm
=====
Delete directories.

Usage
-----
```
rmdir [OPTION]... DIRECTORY...
```

Supported options:
* -h, --help: Show this help message and exit.

All DIRECTORY(ies) are removed. If DIRECTORY is a file or does not exist, it is skipped.

Examples
--------
```
[/]> mkdir test
[/]> cd test
[test]> mkdir test1 test2
[test]> touch test3
[test]> ls
test1/ test2/ test3
[test]> rmdir test1 test2
[test]> ls
test3
[test]> rmdir test3
rmdir: 'test3' is a file!
[test]> ls
test3
[test]> rm test3
[test]> ls

[test]>
```