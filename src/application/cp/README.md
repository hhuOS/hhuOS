cp
=====
Copy files.

Usage
-----
```
cat SOURCE DESTINATION
cat SOURCE... DIRECTORY
```

Copy SOURCE to DESTINATION, or multiple SOURCE files to DIRECTORY.

Supported options:
 * -h, --help: Show this help message and exit.

Examples
--------
```
[/]> echo Hello > file1.txt
[/]> cp file1.txt file2.txt
[/]> cat file2.txt
Hello
```

```
[/]> echo Hello > file1.txt
[/]> echo World > file2.txt
[/]> mkdir mydir
[/]> cp file1.txt file2.txt mydir/
[/]> ls mydir
file1.txt file2.txt
[/]> cat mydir/file1.txt mydir/file2.txt
Hello
World
```