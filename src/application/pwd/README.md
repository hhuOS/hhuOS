pwd
=====
Print the name of the current working directory.

Usage
-----
```
pwd [OPTION]...
```

Supported options:
* -h, --help: Show this help message and exit.

Print the full path of the current working directory.

Examples
--------
```
[/]> pwd
/
[/] cd user
[user] pwd
/user
[user] cd /device/acpi
[acpi] pwd
/device/acpi
```