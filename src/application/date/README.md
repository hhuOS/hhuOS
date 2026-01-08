date
=====
Print the system date and time to the standard output.

Usage
-----
```sh
date [FORMAT]
```

Display date and time in the given FORMAT. Supported format specifiers are:
 * %%: A literal '%' character
 * %a: Abbreviated weekday name (e.g., Mon)
 * %A: Full weekday name (e.g., Monday)
 * %b: Abbreviated month name (e.g., Jan)
 * %B: Full month name (e.g., January)
 * %c: Locale's appropriate date and time representation (e.g. Tue Jan  6 16:43:51 2026)
 * %d: Day of the month (01-31)
 * %H: Hour (00-23)
 * %I: Hour (01-12)
 * %j: Day of the year (001-366)
 * %m: Month (01-12)
 * %M: Minute (00-59)
 * %p: AM or PM designation
 * %S: Second (00-59)
 * %U: Week number of the year (00-53), with Sunday as the first day of the week
 * %w: Weekday as a decimal number (0-6, Sunday=0)
 * %W: Week number of the year (00-53), with Sunday as the first day of the week
 * %x: Locale's appropriate date representation (e.g. 06.01.2026)
 * %X: Locale's appropriate time representation (e.g. 16:43:51)
 * %y: Year without century (00-99)
 * %Y: Year (e.g., 2024)
 * %Z: Time zone name or abbreviation (e.g. UTC+0)

Supported options:
 * -h, --help: Show this help message and exit.

Examples
--------
```sh
[/]> date
Tue Jan  6 16:43:51 UTC 2026
[/]> date "%Y-%m-%d %H:%M:%S"
2026-01-06 16:43:51
[/]> date "It is now %I:%M %p on %A, %B %d, %Y"
It is now 04:43 PM on Tuesday, January 06, 2026
```