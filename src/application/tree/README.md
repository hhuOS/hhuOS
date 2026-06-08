tree
=====
List contents of a directory in a tree-like format.

Usage
-----
```
touch [OPTION]... FILE...
```

Supported options:
* -h, --help: Show this help message and exit.

The given directories are walked recursively.
Each file is printed with its name and multiple '-' signs depicting the directory hierarchy.
If no argument is given, the current directory is used.

Examples
--------
```
[/] mkdir folder
[/] folder/sub1
[/] folder/sub2
[/] touch folder/sub1/file1 folder/sub2/file2
[/] touch filder/sub2/file3 folder/sub2/file4 folder/sub2/file5
[/] tree folder
|-folder
|--sub1/
|---file1
|---file2
|--sub2/
|---file3
|---file4
|---file5
[/] tree folder/sub1 folder/sub2
|-sub1
|--file1
|--file2
|-sub2
|--file3
|--file4
|--file5
```