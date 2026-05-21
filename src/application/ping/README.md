ping
=====
Send ICMP `ECHO_REQUEST` datagrams to network hosts.

Usage
-----
```
ping [OPTION]... DESTINATION
```

Supported options:
* -c, --count: The number of echo request to send (Default: 10).
* -h, --help: Show this help message and exit.

Send ICMP `ECHO_REQUEST` datagrams to the network host specified by DESTINATION (IPv4 address).
If the host answers with an `ECHO_RESPONSE` datagram, the round-trip time is measured and printed to standard out.
The number of iterations can be controlled via the `count` option (default is 10).

Examples
--------
```
[/]> ping -c 5 8.8.8.8
8 bytes from 8.8.8.8 (Sequence number: 0, Time 32ms)
8 bytes from 8.8.8.8 (Sequence number: 0, Time 31ms)
8 bytes from 8.8.8.8 (Sequence number: 0, Time 23ms)
8 bytes from 8.8.8.8 (Sequence number: 0, Time 24ms)
8 bytes from 8.8.8.8 (Sequence number: 0, Time 24ms)
```