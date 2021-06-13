//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_NETWORKPROCESSRESULT_H
#define HHUOS_NETWORKPROCESSRESULT_H


#include <lib/string/String.h>

class NetworkProcessResult {
public:
    enum class ResultType {
        ETH_DELIVER_SUCCESS = 0,
        ETH_FRAME_NULL,
        ETH_DEVICE_NULL,
        ETH_COPY_BYTEBLOCK_FAILED,
        ETH_COPY_BYTEBLOCK_INCOMPLETE,
        IP4_DELIVER_SUCCESS,
        IP4_DATAGRAM_NULL,
        IP4_INTERFACE_NULL,
        IP4_RECEIVER_ADDRESS_NULL,
        IP4_MATCHING_BITS_FUNCTION_BROKEN,
        IP4_NO_ROUTE_FOUND,
        BYTEBLOCK_ACTION_SUCCESS,
        BYTEBLOCK_NETWORK_DEVICE_NULL,
        BYTEBLOCK_BYTES_NULL,
        ARP_RESOLVE_SUCCESS,
        ARP_PROTOCOL_ADDRESS_NULL,
        ARP_TABLE_NULL
    };

private:
    ResultType type;
    String *message = nullptr;

    NetworkProcessResult(ResultType type, String *message);

public:
    ResultType getType() const;

    String *getMessage() const;
};


#endif //HHUOS_NETWORKPROCESSRESULT_H
