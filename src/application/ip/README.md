ip
=====
Show and manipulamodifyte IP addresses and routes.

Usage
-----
```
ip [OPTION]... COMMAND SUBCOMMAND
```

Supported options:
 * -h, --help: Show this help message and exit.

The command and subcommand can be abbreviated. For example, `ip address show`, `ip addr show` and `ip a s` are all equivalent.

Commands
--------
 * **address:** Show and modify IP addresses.
   * **show** [*DEVICE*]: Show the addresses assigned to the specified device.
     If no device is specified, all addresses of all devices are shown.
     ```
     [/]> ip address show eth0
     eth0:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.15/24
     ```
   * **add** *ADDRESS* *DEVICE*: Add an IP address to the specified device.
     ```
     [/]> ip addr show eth1
     eth0:
         MAC: 52:54:00:12:34:57
     [/]> ip addr add 10.0.2.16/24 eth1
     [/]> ip addr show eth1
     eth1:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.16/24
     ```
   * **del** *DEVICE* *ADDRESS*: Remove an IP address from the specified device.
     ```
     [/]> ip addr show eth0
     eth0:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.15/24
     [/]> ip addr del 10.0.2.15/24 eth0
     [/]> ip addr show eth0
     eth0:
         MAC: 52:54:00:12:34:56
     ```
 * **route:** Show and modify IP routes.
   * **show** [*DEVICE*]: Show the routes assigned to the interface with the specified IP address.
     If no IP address is specified, all routes of all interfaces are shown.
     ```
     [/]> ip address show eth0
     eth0:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.15/24
     [/]> ip route show 10.0.2.15
     10.0.2.15/24 device eth0 source 10.0.2.15
     default via 10.0.2.2 device eth0 source 10.0.2.15
     ```
   * **add** [*SOURCE_ADDRESS*] *TARGET_ADDRESS* [*NEXT_HOP*] *DEVICE*: Add a route.
     The target address must be a subnet address (e.g., 10.0.2.0/24).  
     The source address is an optional IP address. If no source address is specified, it is extracted from the target address (e.g., 10.0.2.15/24 -> 10.0.2.15).  
     The next hop is an optional IP address, only relevant if traffic is routed through another network hop.  
     ```
     [/]> ip addr show eth1
     eth0:
         MAC: 52:54:00:12:34:57
     [/]> ip addr add 10.0.2.16/24 eth1
     [/]> ip addr show eth1
     eth1:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.16/24
     [/]> ip route add 10.0.2.16/24 eth1
     [/]> ip route show 10.0.2.16
     10.0.2.16/24 device eth1 source 10.0.2.16
     ```
   * **delete** [*SOURCE_ADDRESS*] *TARGET_ADDRESS* [*NEXT_HOP*] *DEVICE*: Delete a route.  
     The target address must be a subnet address (e.g., 10.0.2.0/24).  
     The source address is an optional IP address. If no source address is specified, it is extracted from the target address (e.g., 10.0.2.15/24 -> 10.0.2.15).  
     The next hop is an optional IP address, only relevant if traffic is routed through another network hop.
     ```
     [/]> ip addr show eth0
     eth0:
         MAC: 52:54:00:12:34:56
         IPv4: 10.0.2.15/24
     [/]> ip route show 10.0.2.15
     10.0.2.15/24 device eth0 source 10.0.2.15
     default via 10.0.2.2 device eth0 source 10.0.2.15
     [/]> ip route delete 10.0.2.15 eth0
     [/]> ip route show 10.0.2.15
     default via 10.0.2.2 device eth0 source 10.0.2.15
     ```