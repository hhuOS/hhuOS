//
// Created by hannes on 16.06.21.
//

#ifndef HHUOS_NETWORKDEFINITIONS_H
#define HHUOS_NETWORKDEFINITIONS_H

#define MAC_SIZE 6
#define ETHERNET_MTU 1500
#define ETHERNETHEADER_MAX_LENGTH 14

#define IP4ADDRESS_LENGTH 4
#define IP4HEADER_MIN_LENGTH 20
#define IP4HEADER_MAX_LENGTH 60 //RFC 791 page 13

#define ECHO_PORT_NUMBER 7 //RFC 862
#define ECHO_INPUT_BUFFER_SIZE 1024
//TODO: Verify values with RFC Sources

#define UDP4DATAPART_MAX_LENGTH 1500
#define UDP4HEADER_MAX_LENGTH 0xffff
#define IP4DATAPART_MAX_LENGTH 576 //Recommended in RFC 791 page 13

#define ARP_WAIT_TIME 100 //Time in milliseconds to wait for ARP Replies

#define UDP_PRIVATE_PORT_MIN 49152
#define UDP_PRIVATE_PORT_MAX 65535 //free private port range defined in RFC 6335 page 11 middle

#define UDP_SOCKET_BUFFER_SIZE 256

#endif //HHUOS_NETWORKDEFINITIONS_H
