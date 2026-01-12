ls
=====
List directory contents.

Usage
-----
```sh
ls [FILE]...
```

Supported options:
 * -h, --help: Show this help message and exit.

If no FILE is specified, list the contents of the current directory.  
If a FILE is specified, list the contents of that directory or print the file name if it's a file.  
If multiple FILEs are specified, list the contents of each FILE in alphabetical order.  

Examples
--------
```sh
[/]> mkdir folder
[/]> ls folder

[/]> touch folder/file1.txt
[/]> touch folder/file2.txt
[/]> ls folder
file1.txt file2.txt
[/]> cd folder
[/]> ls ..
file1.txt file2.txt
[/]> ls file1.txt
file1.txt
```