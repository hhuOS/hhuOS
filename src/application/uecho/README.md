uecho
=====
Start a UDP echo server/client.

Usage
-----
```
uecho [OPTION]...
```

Supported options:
* -s, --server: Start a UDP echo server. 
* -r, --remote ADDRESS: Start a UDP echo client and connect to the server at ADDRESS
* -a, --address ADDRESS: Bind the local socket to ADDRESS (default: 0.0.0.0:1797)
* -h, --help: Show this help message and exit.

Either --server or --remote must be specified.
Specifying a bind address is optional.

Examples
--------
```
[/]> uecho -s &
UDP echo server running on 0.0.0.0:1797! Send 'exit' to leave.
[/] uecho -r 0.0.0.0:1797
UDP echo client running on 0.0.0.0:1024 and sending to 0.0.0.0:1797! Type 'exit' to leave.
Hello, World!
Received: Hello, World!
exit
Received: exit
[/]>
```