rm
=====
Delete files.

Usage
-----
```
rm [OPTION]... FILE...
```

Supported options:
* -h, --help: Show this help message and exit.

All FILEs are removed. If FILE is a directory or does not exist, it is skipped.

Examples
--------
```
[/]> mkdir test
[/]> cd test
[test]> touch test1 test2
[test]> mkdir test3
[test]> ls
test1 test2 test3/
[test]> rm test1 test2
[test]> ls
test3/
[test]> rm test3
rm : 'test3' is a directory!
[test]> ls
test3/
[test]> rmdir test3
[test]> ls

[test]>
```