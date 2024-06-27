# Nettest
nettest is tool developed to test the network stack of hhuOS.

This implementation only supports a running nettest as a server.
The version on hhuOS also supports client mode.

## Sources used
### Sockets
- https://stackoverflow.com/questions/53051590/udp-server-client-c-sendto-recvfrom
  - Last accessed 2024-03-31
- https://www.ibm.com/docs/en/zos/2.4.0?topic=sockets-using-sendto-recvfrom-calls
  - Last accessed 2024-03-31
### Timing
- https://en.cppreference.com/w/cpp/chrono
  - Last accessed 2024-03-31

## Compiling nettest-server
```
g++ nettest-server.cpp -o nettest-server
```

## Starting nettest-server
```
./nettest-server
```
This creates a nettest server listening for incoming client connections on port 1798 on the system.
To change the port it is currently necessary to edit the `.cpp` and recompile the server.
