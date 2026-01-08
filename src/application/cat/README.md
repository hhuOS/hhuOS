cat
=====
Concatenate files and print on the standard output.
If no files are specified, read from standard input.

Usage
-----
```sh
cat [FILE]...
```

Supported options:
 * -h, --help: Show this help message and exit.

Examples
--------
```sh
[/]> echo Hello > file1.txt
[/]> echo World > file2.txt
[/]> cat file1.txt file2.txt
Hello
World
```