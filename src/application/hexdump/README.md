hexdump
=====
Display file contents in hexadecimal format.

Usage
-----
```sh
hexdump [OPTION]... [FILE]...
```

Supported options:
 * -n, --length COUNT: Read at most COUNT bytes.
 * -s, --skip COUNT: Skip the first COUNT bytes.
 * -h, --help: Show this help message and exit.

When no FILE is specified, read standard input.  
When multiple FILEs are specified, they are processed sequentially as if they were concatenated.

Examples
--------
```sh
[/]> echo Hello > file1.txt
[/]> echo World > file2.txt
[/]> hexdump file1.txt
  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII
-------------------------------------------------------------------------------
 00000000 | 48 65 6C 6C 6F 0A                                | Hello.
[/]> hexdump file1.txt file2.txt
  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII
-------------------------------------------------------------------------------
 00000000 | 48 65 6C 6C 6F 0A 57 6F  72 6C 64 0A             | Hello.World.
[/]> hexdump file1.txt file2.txt file1.txt file2.txt
  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII
-------------------------------------------------------------------------------
 00000000 | 48 65 6C 6C 6F 0A 57 6F  72 6C 64 0A 48 65 6C 6C | Hello.World.Hell
 00000010 | 6F 0A 57 6F 72 6C 64 0A                          | o.World.
[/]> hexdump file1.txt file2.txt file1.txt file2.txt -n 10
  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII
-------------------------------------------------------------------------------
 00000000 | 48 65 6C 6C 6F 0A 57 6F  72 6C                   | Hello.Worl
[/]> hexdump file1.txt file2.txt file1.txt file2.txt -n 10 -s 15
  OFFSET  | 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F |   ANSI  ASCII
-------------------------------------------------------------------------------
 00000000 | 6C 6F 0A 57 6F 72 6C 64  0A                      | lo.World.
```