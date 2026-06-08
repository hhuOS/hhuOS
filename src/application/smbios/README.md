smbios
=====
Dump SMBIOS tables.

Usage
-----
```
smbios [OPTION]...
```

Supported options:
* -t, --type: Only display tables of the specified type.
* -h, --help: Show this help message and exit.

If no option is specified, all tables are displayed.
Supported tables are decoded into a human-readable format.
All other tables are displayed as raw bytes.

This application currently supports the following table types:
* BIOS

Examples
--------
```
[/]> smbios -t 0
SMBIOS 2.8 present.

Handle 0x0000, DMI type 00, 26 bytes
BIOS Information
        Vendor: EDK II
        Version: unknown
        Release Date: 02/02/2022
        Address: 0xE8000
        Runtime Size: 96 KiB
        ROM Size: 64 KiB
        Characteristics: 
                ISA is supported
                Targeted content distribution is supported
                UEFI is supported
                System is a virtual machine
        BIOS Revision: 0.0

[/]> smbios -t 17
SMBIOS 2.8 present.

Handle 0x1100, DMI type 17, 40 bytes
Memory Device
        Header and Data:
                11 28 00 11 00 10 FE FF FF FF FF FF 80 00 09 00 
                01 00 07 02 00 00 00 02 00 00 00 00 00 00 00 00 
                00 00 00 00 00 00 00 00 
        Strings:
                DIMM 0
```