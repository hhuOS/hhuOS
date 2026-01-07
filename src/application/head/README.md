head
=====
Write the first part of files to standard output.

Usage
-----
```sh
head [OPTION]... [FILE]...
```

Supported options:
 * -c/--bytes COUNT: Output the first COUNT bytes of each file.
 * -n/--lines COUNT: Output the first COUNT lines of each file (default is 10).
 * -h/--help: Show this help message and exit.

When no FILE is specified, read standard input.  
If no options are given, the first 10 lines of each file are printed.

Examples
--------
```sh
[/]> echo "Hello, World!" > file.txt
[/]> head file.txt
Hello, World!
[/]> head -n 5 file.txt
Hello, World!
[/]> head -c 5 file.txt
Hello
[/]> head -c 10 file.txt
Hello, Wor
[/]> head -c 1 file.txt
H
```

```sh
[/]> echo Hello > file1.txt
[/]> echo World > file2.txt
[/]> head file1.txt file2.txt
==> file1.txt <==
Hello

==> file2.txt <==
World

[/]>
```