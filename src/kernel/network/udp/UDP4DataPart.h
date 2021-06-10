//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_UDP4DATAPART_H
#define HHUOS_UDP4DATAPART_H

//TODO: Verify values with RFC Sources
#define UDP4DATAPART_MAX_LENGTH 1500
#define UDP4HEADER_MAX_LENGTH 0xffff

class UDP4DataPart {
public:
    virtual uint8_t copyTo(NetworkByteBlock *byteBlock) = 0;

    virtual size_t getLengthInBytes() = 0;

    virtual uint8_t parseData(NetworkByteBlock *input) = 0;
};


#endif //HHUOS_UDP4DATAPART_H
